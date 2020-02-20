#pragma once
#include "../plat.h"
#include <QFile>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QApplication>

namespace ai
{
	constexpr n4 bamax = n4max - 25;

	struct http_base
	{
	protected:
		QNetworkAccessManager qnam_;

		void http_finish_no_read(QNetworkReply* reply, funk<void(n4 status_code, const arr<Header>& headers)> f_response)
		{
            QObject::connect(reply, &QNetworkReply::finished, QApplication::instance(), [f_response, reply]()
			{
				QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
				if (!statusCode.isValid())
				{
					auto errorString = reply->errorString().toStdString();
					delete reply;
					f_response(0, {});
					return;
				}

				auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
				auto headers = arr<Header>();
				std::for_each(reply->rawHeaderPairs().cbegin(), reply->rawHeaderPairs().cend(), [&headers](auto&& h)
				{
					headers.push_back(Header(str(h.first.cbegin(), h.first.cend()), str(h.second.cbegin(), h.second.cend())));
				});
				delete reply;
				f_response(code, headers);
			});
		}

		void http_finish(QNetworkReply* reply, funk<void(n4 status_code, const arr<Header>& headers, str data)> f_response)
		{
            QObject::connect(reply, &QNetworkReply::finished, QApplication::instance(), [f_response, reply]()
			{
				QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
				if (!statusCode.isValid())
				{
					auto errorString = reply->errorString().toStdString();
					delete reply;
					f_response(0, {}, "");
					return;
				}

				auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
				auto headers = arr<Header>();
				std::for_each(reply->rawHeaderPairs().cbegin(), reply->rawHeaderPairs().cend(), [&headers](auto&& h)
				{
					headers.push_back(Header(str(h.first.cbegin(), h.first.cend()), str(h.second.cbegin(), h.second.cend())));
				});
				auto body = reply->read(bamax);
                auto sbody = str(body.data(), static_cast<sz>(body.size()));
				delete reply;
				f_response(code, headers, move(sbody));
			});
		}
		
		static n8 content_length_from_headers(const arr<Header>& headers)
		{
			n8 content_length = 0;
			std::find_if(headers.cbegin(), headers.cend(), [&content_length](auto&& header)
			{
				auto header_name = header.name;
				std::transform(header_name.cbegin(), header_name.cend(), header_name.begin(), [](char c) { return 'A' <= c && c <= 'Z' ? c - 'A' + 'a' : c; });
				if ("content-length" == header_name)
				{
					content_length = std::stoll(header.value);
					return true;
				}
				return false;
			});
			return content_length;
		}
		void add_http_headers(QNetworkRequest& request, const arr<Header>& headers)
		{
			for_each(headers.cbegin(), headers.cend(), [&request](auto&& h)
			{
				request.setRawHeader(QByteArray::fromStdString(h.name), QByteArray::fromStdString(h.value));
			});
		}
	};

	struct http_writer : http_base
	{
		void http_request
		(
			HttpVerb verb,
			const str& url,
			const arr<Header>& headers,
			str body,
			funk<void(n4 status_code, const arr<Header>& headers, str data)> f_response
		)
		{
			auto request = QNetworkRequest(QUrl(QString::fromStdString(url)));

			QByteArray payload;
			if (!body.empty() && HttpVerb::REMOVE != verb && HttpVerb::GET != verb && HttpVerb::OPTIONS != verb)
			{
				payload = QByteArray::fromStdString(body);
				request.setRawHeader("Content-Length", QByteArray::number(payload.size()));
			}

			add_http_headers(request, headers);

			QNetworkReply* reply = nullptr;
			switch (verb)
			{
			case HttpVerb::GET: reply = qnam_.get(request); break;
			case HttpVerb::REMOVE: reply = qnam_.deleteResource(request); break;
			case HttpVerb::OPTIONS: reply = qnam_.sendCustomRequest(request, "OPTIONS"); break;
			case HttpVerb::PATCH: reply = qnam_.sendCustomRequest(request, "PATCH", payload); break;
			case HttpVerb::PUT: reply = qnam_.put(request, payload); break;
			default: reply = qnam_.post(request, payload); break;
			}

			http_finish(reply, f_response);
		}

		void http_upload_file
		(
			HttpVerb verb,
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			funk<void(Progress)> f_progress,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
		)
		{
			assert(!file_path.empty() && HttpVerb::REMOVE != verb && HttpVerb::GET != verb && HttpVerb::OPTIONS != verb);

			auto request = QNetworkRequest(QUrl(QString::fromStdString(url)));
			add_http_headers(request, headers);

			uptr<QFile> file = std::make_unique<QFile>(QString::fromStdString(file_path));
			if (!file->open(QIODevice::ReadOnly) || file->size() < 0)
			{
				fun(0, {}, 0);
				return;
			}

			QNetworkReply* reply = nullptr;
			switch (verb)
			{
			case HttpVerb::PUT: reply = qnam_.put(request, file.get()); break;
			case HttpVerb::POST: reply = qnam_.post(request, file.get()); break;
			default: reply = qnam_.sendCustomRequest(request, "PATCH", file.get()); break;
			}

			QObject::connect(reply, &QNetworkReply::uploadProgress, QApplication::instance(), [f_progress](auto received, auto total)
			{
				f_progress({ received, total });
			});

			http_finish_no_read(reply, [file = file.get(), fun](n4 sc, auto& headers)
			{
				auto ufile = uptr<QFile>(file);
				fun(sc, headers, content_length_from_headers(headers));
			});
			file.release();
		}

	};

	struct http_reader : http_base
	{
		void http_download
		(
			const str& url,
			par<p8, p8>,
			const arr<Header>& headers,
			funk<void(n4 status_code, const arr<Header>& headers, str data)> f_response
		)
		{
			auto request = QNetworkRequest(QUrl(QString::fromStdString(url)));
			add_http_headers(request, headers);
			auto reply = qnam_.get(request);
			http_finish(reply, f_response);
		}

		void http_download_file
		(
			const str& url,
			const str& file_path,
			const arr<Header>& hh,
			n8 size,
			p16 md5,
			funk<void(Progress)> f_progress,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
		)
		{
			assert(!file_path.empty());

			auto request = QNetworkRequest(QUrl(QString::fromStdString(url)));

			auto headers = hh;

			QFileInfo fileinfo(QString::fromStdString(file_path));
			auto flag = QIODevice::Append;
			if (fileinfo.exists())
			{
                if (static_cast<sz>(fileinfo.size()) == size)
				{
					fun(200, {}, size);
					return;
				}
				if (fileinfo.size() < size)
				{
					headers.push_back({ "Range", "bytes=" + std::to_string(fileinfo.size()) + "-" });
					if (md5.first || md5.second)
						headers.push_back({ "If-Match", to_hex(md5) });
				}
				else
					flag = QIODevice::Truncate;
			}

			add_http_headers(request, headers);

			sptr<QFile> file = std::make_shared<QFile>(QString::fromStdString(file_path));
			if (!file->open(QIODevice::WriteOnly | flag))
			{
				fun(0, {}, 0);
				return;
			}

			QNetworkReply* reply = qnam_.get(request);

			QObject::connect(reply, &QNetworkReply::downloadProgress, QApplication::instance(), [f_progress](auto received, auto total)
			{
				f_progress({ received, total });
			});

			QObject::connect(reply, &QNetworkReply::readyRead, QApplication::instance(), [file, reply]()
			{
				auto data = reply->readAll();
				auto bytes_written = file->write(data);
				assert(bytes_written == data.size());
				bytes_written = 0;
			});
			http_finish_no_read(reply, [file, fun](n4 sc, auto& headers)
			{
				fun(sc, headers, content_length_from_headers(headers));
			});
		}
	};

	struct qt_plat : plat
	{
		str app_data_location() final
		{
			return app_data_location_;
		}

		void file_delete(str path, funk<void(bool)> fun) final
		{
			add_task([path, fun]()
			{
				bool r = false;

				{
					QFile f(QString::fromStdString(path));
					r = f.remove();
				}
				fun(r);
			});
		}

		str current_working_directory() final
		{
			return QDir::currentPath().toStdString();
		}

		void file_readall(const str& path, funk<void(str)> fun) final
		{
			add_task([this, path, fun]()
			{
				auto r = str();

				{
					QFile f(QString::fromStdString(path));
					if (!f.open(QFile::ReadOnly | QFile::Text))
					{
						fun("");
						return;
					}
					QTextStream in(&f);
					r = in.readAll().toStdString();
				}

				fun(std::move(r));
			});
		}

		void stop_taking_tasks() final
		{
			add_task([this]() { stopping_ = true; });
		}

		void file_size(const str& path, funk<void(n8)> fun) final
		{
			add_task([this, path, fun]()
			{
				QFileInfo info(QString::fromStdString(path));
				fun(info.size());
			});
		}

		void copy_file(const str& current_path, const str& new_path, funk<void(bool)> fun) final
		{
			add_task([this, current_path, new_path, fun]()
			{
				auto process = [current_path](QFile& writer)
				{
					QFile reader(QString::fromStdString(current_path));
					if (!reader.open(QIODevice::ReadOnly))
						return false;

					n8 size_left = reader.size();
					while (size_left > 0)
					{
						auto chunk = std::min<n8>(size_left, MB);
						auto old_data = reader.read(chunk);
						auto size_written = writer.write(old_data);
						if (size_written != old_data.size())
							return false;
						size_left -= chunk;
					}
					return true;
				};

				auto r = false;

				{
					QFile writer(QString::fromStdString(new_path));

					if (writer.open(QIODevice::WriteOnly | QIODevice::Truncate))
						r = process(writer);
					else
					{
						auto it = find(new_path.crbegin(), new_path.crend(), '/');
						auto new_dir = str(new_path.cbegin(), it.base());
						make_path(new_dir, [process, fun, new_path](auto stat)
						{
							if (!stat)
							{
								fun(stat);
								return;
							}

							auto r = false;

							{
								QFile writer(QString::fromStdString(new_path));
								if (!writer.open(QIODevice::NewOnly))
									r = false;
								else
									r = process(writer);
							}

							fun(r);
						});
						return;
					}
				}
				fun(r);
			});
		}

		void make_path(const str& path, funk<void(bool)> fun) final
		{
			add_task([path, fun]()
			{
				fun(QDir().mkpath(QString::fromStdString(path)));
			});
		}

		void rename_file(const str& path, const str& new_path, funk<void(bool)> fun) final
		{
			add_task([this, path, new_path, fun]()
			{
				QFile file(QString::fromStdString(path));
				auto r = file.rename(QString::fromStdString(new_path));
				fun(r);
				//QFileInfo info(QString::fromStdString(new_path));
				//fun(info.exists() ? status::conflict : status::io);
			});
		}

		void finish_io_and_wait() final
		{
			stop_taking_tasks();
			worker_->wait();
		}

		void http_request
		(
			HttpVerb verb,
			const str& url,
			const arr<Header>& headers,
			str body,
			funk<void(n4 status_code, const arr<Header>& headers, str data)> f_response
		) final
		{
			http_writer_.http_request(verb, url, headers, move(body), f_response);
		}

		void http_upload_file
		(
			HttpVerb verb,
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			funk<void(Progress)> f_progress,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> f_response
		) final
		{
			http_writer_.http_upload_file(verb, url, file_path, headers, f_progress, f_response);
		}

		void http_download_file
		(
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			n8 size,
			p16 md5,
			funk<void(Progress)> f_progress,
			funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
		) final
		{
			http_reader_.http_download_file(url, file_path, headers, size, md5, f_progress, fun);
		}

		qt_plat(const str& data_location) : app_data_location_(data_location) {}
	private:
		str app_data_location_;
		QThread* worker_ = nullptr;
		arr<funk<void()>> tasks_;
		std::mutex mutex_;
		bool stopping_ = false;
		http_reader http_reader_;
		http_writer http_writer_;

		void add_task(funk<void()> fun)
		{
			gate x(mutex_);
			tasks_.push_back(fun);
			get_busy_if_needed();
		}

		funk<void()> next()
		{
			gate x(mutex_);
			if (0 == tasks_.size())
				return nullptr;
			auto r = tasks_[tasks_.size() - 1];
			tasks_.pop_back();
			return r;
		}

		void get_busy_if_needed()
		{
			if (worker_ || 0 == tasks_.size())
				return;
			worker_ = QThread::create([this]()
			{
				while (true)
				{
					while (true)
					{
						if (stopping_)
							return;
						auto task = next();
						if (!task)
							break;
						task();
					}
					if (stopping_)
						return;
					{
						gate x(mutex_);
						if (0 == tasks_.size())
						{
							worker_->deleteLater();
							worker_ = nullptr;
							break;
						}
					}
				}
			});
			worker_->start();
		}
	};
}
