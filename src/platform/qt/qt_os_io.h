#pragma once
#include "../../bridge/cpp/os_io.hpp"
#include "../../bridge/cpp/long_result.hpp"
#include "../../bridge/cpp/bool_result.hpp"
#include "../../bridge/cpp/http_result.hpp"
#include "../../bridge/cpp/http_verb.hpp"
#include "../../bridge/cpp/http_progress_result.hpp"
#include "../../bridge/cpp/header.hpp"
#include "../../bridge/cpp/progress.hpp"
#include "../../bridge/cpp/string_result.hpp"
#include "../../bridge/cpp/portal.hpp"
#include "../../bridge/cpp/binary_result.hpp"
#include "../../bridge/cpp/cancellation_token.hpp"

#include "../../general.h"
#include <QApplication>
#include <QTimer>

#include <QFile>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QMainWindow>
#include "qtdtmainwindow.h"
#include <QDirIterator>
#include <iostream>

namespace ai
{
	constexpr n4 bamax = n4max - 25;

	struct network_cancellation_token : CancellationToken
	{
		virtual ~network_cancellation_token()
		{
		}
		void cancel() final
		{
			QTimer::singleShot(0, QApplication::instance(), [weak_reply = wptr<QNetworkReply>(reply_)]()
			{
				auto reply = weak_reply.lock();
				if (reply)
					reply->abort();
			});
		}
		QNetworkReply* reply() const { return reply_.get(); }
		void setReplyOnGuiThread(QNetworkReply* r)
		{
			assert(!reply_);
			reply_ = sptr<QNetworkReply>(r);
		}
	private:
		sptr<QNetworkReply> reply_;
	};

	struct http_base : QObject
	{
		http_base(QApplication& app) : app_(app)
		{
		}
	protected:
		void http_finish_no_read(const sptr<network_cancellation_token>& cancellation, funk<void(p4 status_code, const arr<Header>& headers)> fun)
		{
			connect(cancellation->reply(), &QNetworkReply::finished, [fun, cancel = wptr<network_cancellation_token>(cancellation)]()
			{
				auto cancellation = cancel.lock();
				if (!cancellation)
					return;

				auto reply = cancellation->reply();
				QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
				if (QNetworkReply::NoError != reply->error() || !statusCode.isValid())
				{
					auto errorString = reply->errorString().toStdString();
					fun(0, {});
					return;
				}
				auto headers = arr<Header>();
				std::for_each(reply->rawHeaderPairs().cbegin(), reply->rawHeaderPairs().cend(), [&headers](auto&& h)
				{
					headers.push_back(Header(str(h.first.cbegin(), h.first.cend()), str(h.second.cbegin(), h.second.cend())));
				});
				fun(statusCode.toUInt(), headers);
			});
		}

		void http_finish(const sptr<network_cancellation_token>& cancellation, const sptr<HttpResult>& result)
		{
			connect(cancellation->reply(), &QNetworkReply::finished, [result, cancel = wptr<network_cancellation_token>(cancellation)]()
			{
				auto cancellation = cancel.lock();
				if (!cancellation)
					return;

				auto reply = cancellation->reply();
				QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
				if (QNetworkReply::NoError != reply->error() || !statusCode.isValid())
				{
					auto errorString = reply->errorString().toStdString();
					result->on_result(0, {}, "");
					return;
				}
				auto headers = arr<Header>();
				std::for_each(reply->rawHeaderPairs().cbegin(), reply->rawHeaderPairs().cend(), [&headers](auto&& h)
				{
					headers.push_back(Header(str(h.first.cbegin(), h.first.cend()), str(h.second.cbegin(), h.second.cend())));
				});
				auto body = reply->read(bamax);
				auto sbody = str(body.data(), body.size());
				result->on_result(statusCode.toUInt(), headers, move(sbody));
			});
		}

		static n8 content_length_from_headers(const arr<Header>& headers)
		{
			n8 content_length = 0;
			std::find_if(headers.cbegin(), headers.cend(), [&content_length](auto&& header)
			{
				auto header_name = header.name;
				std::transform(header.name.cbegin(), header.name.cend(), header_name.begin(), [](char c) { return 'A' <= c && c <= 'Z' ? c - 'A' + 'a' : c; });
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
		QNetworkAccessManager qnam_;
		QApplication& app_;
	private:
	};

	struct http_writer : http_base
	{
		http_writer(QApplication& app) : http_base(app) {}

		sptr<CancellationToken> http_request
		(
			HttpVerb verb,
			const str& url,
			const arr<Header>& headers,
			const str& body,
			const sptr<HttpResult>& result
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


			auto cancellation = std::make_shared<network_cancellation_token>();
			QTimer::singleShot(0, &app_, [this, verb, request, payload, result, cancellation]()
			{
				switch (verb)
				{
				case HttpVerb::GET: cancellation->setReplyOnGuiThread(qnam_.get(request)); break;
				case HttpVerb::REMOVE: cancellation->setReplyOnGuiThread(qnam_.deleteResource(request)); break;
				case HttpVerb::OPTIONS: cancellation->setReplyOnGuiThread(qnam_.sendCustomRequest(request, "OPTIONS")); break;
				case HttpVerb::PATCH: cancellation->setReplyOnGuiThread(qnam_.sendCustomRequest(request, "PATCH", payload)); break;
				case HttpVerb::PUT: cancellation->setReplyOnGuiThread(qnam_.put(request, payload)); break;
				default: cancellation->setReplyOnGuiThread(qnam_.post(request, payload)); break;
				}

				http_finish(cancellation, result);
			});

			return cancellation;
		}

		sptr<CancellationToken> http_upload_file
		(
			HttpVerb verb,
			const str& url,
			const str& file_path,
			const arr<Header>& headers,
			const sptr<HttpProgressResult>& result
		)
		{
			assert(!file_path.empty() && HttpVerb::REMOVE != verb && HttpVerb::GET != verb && HttpVerb::OPTIONS != verb);

			auto request = QNetworkRequest(QUrl(QString::fromStdString(url)));
			add_http_headers(request, headers);

			sptr<QFile> file = std::make_shared<QFile>(QString::fromStdString(file_path));
			if (!file->open(QIODevice::ReadOnly) || file->size() < 0)
			{
				result->on_result(0, {}, 0);
				return nullptr;
			}

			auto cancellation = std::make_shared<network_cancellation_token>();
			QTimer::singleShot(0, &app_, [this, verb, request, result, file, cancellation]()
			{
				switch (verb)
				{
				case HttpVerb::PUT: cancellation->setReplyOnGuiThread(qnam_.put(request, file.get())); break;
				case HttpVerb::POST:cancellation->setReplyOnGuiThread(qnam_.post(request, file.get())); break;
				default: cancellation->setReplyOnGuiThread(qnam_.sendCustomRequest(request, "PATCH", file.get())); break;
				}

				QObject::connect(cancellation->reply(), &QNetworkReply::uploadProgress, QApplication::instance(), [result](auto received, auto total)
				{
					result->on_progress(Progress(received, total));
				});

				http_finish_no_read(cancellation, [file, result](p4 sc, auto& headers)
				{
					result->on_result(sc, headers, file->size());
				});
			});

			return cancellation;
		}
	};

	struct http_reader : http_base
	{
		http_reader(QApplication& app) : http_base(app) {}
		
		sptr<CancellationToken> http_download_file
		(
			const str& url,
			const str& file_path,
			const arr<Header>& hh,
			n8 size,
			const str& md5,
			const sptr<HttpProgressResult>& result
		)
		{
			assert(!file_path.empty());

			auto request = QNetworkRequest(QUrl(QString::fromStdString(url)));

			auto headers = hh;

			QFileInfo fileinfo(QString::fromStdString(file_path));
			auto flag = QIODevice::Append;
			n8 bytes_downloaded = 0;
			if (fileinfo.exists())
			{
				if (fileinfo.size() == size)
				{
					result->on_result(200, {Header("etag", md5)}, size);
					return nullptr;
				}
				if (fileinfo.size() < size)
				{
					bytes_downloaded = fileinfo.size();
					headers.push_back(Header( "Range", "bytes=" + std::to_string(fileinfo.size()) + "-" ));
					if (!md5.empty())
						headers.push_back(Header( "If-Match", md5 ));
				}
				else
					flag = QIODevice::Truncate;
			}

			add_http_headers(request, headers);

			sptr<QFile> file = std::make_shared<QFile>(QString::fromStdString(file_path));
			if (!file->open(QIODevice::WriteOnly | flag))
			{
				result->on_result(0, {}, 0);
				return nullptr;
			}

			auto cancellation = std::make_shared<network_cancellation_token>();
			QTimer::singleShot(0, &app_, [this, request, result, file, bytes_downloaded, cancellation]()
			{
				cancellation->setReplyOnGuiThread(qnam_.get(request));

				auto reply = cancellation->reply();
				QObject::connect(reply, &QNetworkReply::downloadProgress, QApplication::instance(), [result, bytes_downloaded](auto received, auto total)
				{
					result->on_progress({ received + bytes_downloaded, total + bytes_downloaded });
				});

				QObject::connect(reply, &QNetworkReply::readyRead, QApplication::instance(), [file, cancel = wptr<network_cancellation_token>(cancellation)]()
				{
					auto cancellation = cancel.lock();
					if (!cancellation)
						return;
					auto data = cancellation->reply()->readAll();
					auto chars = data.data();
					int bytes_written = 0;
					while (bytes_written != data.size())
					{
						bytes_written += file->write(chars + bytes_written, data.size() - bytes_written);
						if (bytes_written != data.size())
							std::cout << "probleem\n";
					}
				});
				http_finish_no_read(cancellation, [result](p4 sc, auto& headers)
				{
					result->on_result(sc, headers, content_length_from_headers(headers));
				});
			});

			return cancellation;
		}
	};
		
	struct qt_os_io : OsIo
	{
		qt_os_io(QApplication& app) : http_writer_(app), http_reader_(app)
		{
		}

		void log(LogType type, int32_t line, const std::string & file, const std::string & message) final
		{
			switch (type)
			{
			case LogType::LERROR: std::cerr << "ERROR " << message << " (" << line << ") " << file << std::endl; break;
			default:
				switch (type)
				{
				case LogType::LDEBUG: std::cout << "DEBUG"; break;
				case LogType::LVERBOSE: std::cout << "VERBOSE"; break;
				case LogType::LWARNING: std::cout << "WARNING"; break;
				default: std::cout << "INFO"; break;
				}
				std::cout << " " << message << " (" << line << ") " << file << std::endl; 
				break;
			}
		}

		void file_readall(const std::string & path, const std::shared_ptr<BinaryResult> & result) final
		{
			add_task([path, result]()
			{
				auto r = arr<p1>();
				{
					QFile reader(QString::fromStdString(path));
					if (!reader.open(QIODevice::ReadOnly))
					{
						result->on_result({});
						return;
					}
					auto data = reader.readAll();
					r.resize(data.size());
					std::transform(data.cbegin(), data.cend(), r.begin(), [](char c) { return static_cast<p1>(c); });
				}
				result->on_result(r);
			});
		}

		void file_writeall(const std::string & path, const std::vector<uint8_t> & content, const std::shared_ptr<BoolResult> & result) final
		{
			add_task([path, content, result]()
			{
				bool success = false;

				{
					QFile writer(QString::fromStdString(path));
					if (!writer.open(QIODevice::WriteOnly | QIODevice::Truncate))
					{
						result->on_result(false);
						return;
					}
					auto size_written = writer.write(reinterpret_cast<const char*>(content.data()), content.size());
					success = content.size() == size_written;
				}

				result->on_result(success);
			});
		}

		void file_writeall_safely(const std::string & path, const std::vector<uint8_t> & content, const std::shared_ptr<BoolResult> & result) final
		{
			add_task([path, content, result]()
			{
				{
					QFile writer(QString::fromStdString(path + ".bak"));
					if (!writer.open(QIODevice::WriteOnly | QIODevice::Truncate))
					{
						result->on_result(false);
						return;
					}
					auto size_written = writer.write(reinterpret_cast<const char*>(content.data()), content.size());
					if (content.size() != size_written)
					{
						result->on_result(false);
						return;
					}

					auto qpath = QString::fromStdString(path);

					{
						QFile old(qpath);
						old.remove();
					}

					writer.rename(qpath);
				}

				result->on_result(true);
			});
		}

		void file_write_password(const std::string & username, const std::string & password, const std::shared_ptr<BoolResult> & result) final
		{
			add_task([result]()
			{
				result->on_result(true);
			});
		}

		void file_read_password(const std::string & username, const std::shared_ptr<StringResult> & result) final
		{
			add_task([result]()
			{
				result->on_result("");
			});
		}

		void file_size(const std::string & path, const std::shared_ptr<LongResult> & result) final
		{
			add_task([path, result]()
			{
				QFileInfo info(QString::fromStdString(path));
				result->on_result(info.size());
			});

		}

		bool file_thumbnail(const std::string & path, FileType type, const std::shared_ptr<BinaryResult> & result) final
		{
			return false;
		}

		void copy_file(const std::string & current_path, const std::string & new_path, const std::shared_ptr<BoolResult> & result) final
		{
			add_task([this, current_path, new_path, result]()
			{
				unsafe_copy_file(current_path, new_path, result);
			});
		}

		void make_path(const std::string & path, const std::shared_ptr<BoolResult> & result) final
		{
			make_path_lambda(path, [result](bool success)
			{
				result->on_result(success);
			});
		}

		void rename_file(const std::string & current_path, const std::string & new_path, const std::shared_ptr<BoolResult> & result) final
		{
			add_task([current_path, new_path, result]()
			{
				QFile file(QString::fromStdString(current_path));
				auto r = file.rename(QString::fromStdString(new_path));
				result->on_result(r);
				//QFileInfo info(QString::fromStdString(new_path));
				//fun(info.exists() ? status::conflict : status::io);
			});
		}

		std::shared_ptr<CancellationToken> http_request(HttpVerb verb, const std::string & url, const std::vector<Header> & headers, const std::string & body, const std::shared_ptr<HttpResult> & result) final
		{
			return http_writer_.http_request(verb, url, headers, body, result);
		}

		std::shared_ptr<CancellationToken> http_upload_file(HttpVerb verb, const std::string & url, const std::string & file_path, const std::vector<Header> & headers, const std::shared_ptr<HttpProgressResult> & result) final
		{
			return http_writer_.http_upload_file(verb, url, file_path, headers, result);
		}

		std::shared_ptr<CancellationToken> http_download_file(const std::string & url, const std::string & file_path, const std::vector<Header> & headers, int64_t size, const std::string & md5, const std::shared_ptr<HttpProgressResult> & result) final
		{
			return http_reader_.http_download_file(url, file_path, headers, size, md5, result);
		}

		void wait(int32_t millis, const std::shared_ptr<VoidResult> & result) final
		{
			QTimer::singleShot(std::chrono::milliseconds(millis), QApplication::instance(), [result]()
			{
				result->on_result();
			});
		}

		void file_expand_directories(const std::vector<std::string> & paths, const std::shared_ptr<StringsResult> & result) final
		{
			add_task([paths, result]()
			{
				auto r = arr<str>();
				std::for_each(paths.cbegin(), paths.cend(), [result, &r](const str& path)
				{
					auto qpath = QString::fromStdString(path);
					QFileInfo info(qpath);
					if (info.isDir())
					{
						QDirIterator it(qpath, QDir::Files, QDirIterator::Subdirectories);
						while (it.hasNext())
							r.push_back(it.next().toStdString());
					}
					else
						r.push_back(path);
				});
				result->on_result(r);
			});
		}

		void file_copy_hierarchy(const std::string & dest_root_path, const std::vector<std::string> & dest_relative_paths, const std::vector<std::string> & src_paths) final
		{
			add_task([this, dest_root_path, dest_relative_paths, src_paths]()
			{
				for (int i = 0; i < src_paths.size(); ++i)
				{
					unsafe_copy_file(src_paths[i], dest_root_path + "/" + dest_relative_paths[i], nullptr);
				}
			});
		}

		void make_path_lambda(const str& path, funk<void(bool)> fun)
		{
			add_task([path, fun]()
			{
				fun(QDir().mkpath(QString::fromStdString(path)));
			});
		}

		virtual ~qt_os_io()
		{
			stop_taking_tasks();
			worker_->wait();
		}

	private:
		http_writer http_writer_;
		http_reader http_reader_;
		QThread* worker_ = nullptr;
		arr<funk<void()>> tasks_;
		std::mutex mutex_;
		bool stopping_ = false;

		void stop_taking_tasks()
		{
			add_task([this]() { stopping_ = true; });
		}

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

		void unsafe_copy_file(const str& current_path, const str& new_path, const sptr<BoolResult>& result)
		{
			auto process = [current_path](QFile& writer)
			{
				QFile reader(QString::fromStdString(current_path));
				if (!reader.open(QIODevice::ReadOnly))
					return false;

				auto size_left = reader.size();
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
					make_path_lambda(new_dir, [process, result, new_path](auto stat)
					{
						if (!stat)
						{
							if (result)
								result->on_result(false);
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

						if (result)
							result->on_result(r);
					});
					return;
				}
			}

			if (result)
				result->on_result(r);

		}
	};
}
