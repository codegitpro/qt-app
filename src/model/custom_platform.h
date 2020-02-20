#pragma once
#import "../platform/device_platform.h"
#import "../zz_algorithms.h"
#import <mutex>
#include "../bridge/cpp/transaction_detail.hpp"
#include "../bridge/cpp/transaction_state.hpp"
#include "../bridge/cpp/product.hpp"

namespace ai
{
    struct custom_platform : device_platform
    {
        custom_platform(const str& data_location, const sptr<OsIo>& os)
            :
            device_platform(data_location, os),
            data_location_(data_location)
        {
            logging_enabled_ = false;
        }
        bool logging_enabled()
        {
            std::lock_guard<std::mutex> lock(mutex_);

            return logging_enabled_;
        }
        void enable_logging(bool enabled)
        {
            std::lock_guard<std::mutex> lock(mutex_);

            logging_enabled_ = enabled;
            if (!enabled)
            {
                log_content_.clear();
                write_log();
            }
            else
            {
                read_log();
            }
        }
        static str product_to_string(const Product& product)
        {
            auto r = str();
            r += "id: " + product.product_id + "\n";
            r += "title: " + product.title + "\n";
            r += "description: " + product.product_description + "\n";
            r += "price: " + std::to_string(product.price) + "\n";
            r += "formatted price: " + product.formatted_price + "\n";
            return r;
        }

        void log_products(const std::vector<Product> & products)
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (!logging_enabled_)
                return;
            
            log_content_ += current_time_and_date() + "\n";
            log_content_ += "Products\n";
            for (auto& product : products)
            {
                log_content_ += product_to_string(product);
            }
            log_content_ += "\n";
            write_log();
        }
        
        static str transaction_state_to_string(TransactionState state)
        {
            switch (state)
            {
                case TransactionState::ERROR: return "error";
                case TransactionState::BUSY: return "busy";
                case TransactionState::PURCHASED: return "purchased";
                case TransactionState::RESTORED: return "restored";
                case TransactionState::VERIFY_BLOMP: return "verify_blomp";
            }
            return "UNKNOWN";
        }
        
        static str transaction_to_string(const TransactionDetail& transaction)
        {
            auto r = transaction_state_to_string(transaction.state);
            if (transaction.error_message.empty())
                return r;
            
            return r + " " + transaction.error_message;
        }
        
        void log_transactions(const std::unordered_map<std::string, TransactionDetail> & states)
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (!logging_enabled_)
                return;
            
            log_content_ += current_time_and_date() + "\n";
            log_content_ += "Transactions\n";
            for (auto& item : states)
            {
                log_content_ += item.first + " " + transaction_to_string(item.second) + "\n";
            }
            log_content_ += "\n";
            write_log();
        }
        
        void log(LogType type, int line, const char* file, const arr<str>& messages) final
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (!logging_enabled_)
                return;
            
            device_platform::log(type, line, file, messages);
            
            log_content_ += current_time();
            log_content_ += " ";
            log_content_ += std::to_string(line);
            log_content_ += " ";
            log_content_ += file;
            log_content_ += "\n";
            for (auto& message : messages)
            {
                log_content_ += message;
                log_content_ += "\n";
            }
            log_content_ += "\n\n";
            write_log();
        }
        
        void log(LogType type, int line, const char* file, const str& message) final
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (!logging_enabled_)
                return;
            
            device_platform::log(type, line, file, message);
            
            log_content_ += current_time() + " " + std::to_string(line) + " " + file + "\n";
            log_content_ += message + "\n\n";
            write_log();
        }

        str log_content()
        {
            std::lock_guard<std::mutex> lock(mutex_);

            return log_content_;
        }

        sptr<CancellationToken> http_upload_file
        (
            HttpVerb verb,
            const str& url,
            const str& file_path,
            const arr<Header>& headers,
            funk<void(Progress)> progress_fun,
            funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
        ) final
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (!logging_enabled_)
                    return device_platform::http_upload_file(verb, url, file_path, headers, progress_fun, fun);
            }
            
            str content = current_time_and_date() + "\n";
            content += verb_to_string(verb) + " " + url + "\n";
            content += "path: " + file_path + "\n";
            content += headers_to_string(headers);

            return device_platform::http_upload_file(verb, url, file_path, headers, progress_fun, [this, content = std::move(content), fun](n4 status_code, const arr<Header>& headers, n8 content_length)
            {
                fun(status_code, headers, content_length);

                std::lock_guard<std::mutex> lock(mutex_);
                if (!logging_enabled_)
                    return;
    
                log_content_ += content;
                log_content_ += current_time_and_date() + "\n";
                log_content_ += std::to_string(status_code) + "\n";
                log_content_ += headers_to_string(headers);
                log_content_ += "content length: " + std::to_string(content_length) + "\n";
                log_content_ += "\n";
                write_log();
            });
        }

        sptr<CancellationToken> http_download_file
        (
            const str& url,
            const str& file_path,
            const arr<Header>& headers,
            n8 size,
            p16 md5,
            funk<void(Progress)> progress_fun,
            funk<void(n4 status_code, const arr<Header>& headers, n8 content_length)> fun
        ) final
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (!logging_enabled_)
                    return device_platform::http_download_file(url, file_path, headers, size, md5, progress_fun, fun);
            }

            str content = current_time_and_date() + "\n";
            content += "url: " + url + "\n";
            content += "path: " + file_path + "\n";
            content += "size: " + std::to_string(size) + "\n";
            content += "md5: " + to_hex(md5) + "\n";
            content += headers_to_string(headers);
            
            return device_platform::http_download_file(url, file_path, headers, size, md5, progress_fun, [this, content = std::move(content), fun](n4 status_code, const arr<Header>& headers, n8 content_length)
            {
                fun(status_code, headers, content_length);
                
                std::lock_guard<std::mutex> lock(mutex_);
                if (!logging_enabled_)
                    return;
    
                log_content_ += content;
                log_content_ += current_time_and_date() + "\n";
                log_content_ += std::to_string(status_code) + "\n";
                log_content_ += headers_to_string(headers);
                log_content_ += "content length: " + std::to_string(content_length) + "\n";
                log_content_ += "\n";
                write_log();
            });
        }

        static str headers_to_string(const arr<Header>& headers)
        {
            auto r = str();
            for (auto& header : headers)
            {
                r += header.name + ": " + header.value + "\n";
            }
            return r;
        }
        
        sptr<CancellationToken> http_request
        (
            HttpVerb verb,
            const str& url,
            const arr<Header>& headers,
            str body,
            funk<void(n4 status_code, const arr<Header>& headers, str data)> fun
        ) final
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (!logging_enabled_)
                    return device_platform::http_request(verb, url, headers, body, fun);
            }
            
            str content = current_time_and_date() + "\n";
            content += verb_to_string(verb) + " " + url + "\n";
            content += headers_to_string(headers);
            if (!body.empty())
            {
                content += "<<\n";
                content += body + "\n";
                content += ">>\n";
            }
            return device_platform::http_request(verb, url, headers, body, [this, fun, content = std::move(content)](n4 status_code, const arr<Header>& headers, str data)
            {
                fun(status_code, headers, data);
                
                std::lock_guard<std::mutex> lock(mutex_);
                if (!logging_enabled_)
                    return;
    
                log_content_ += content;
                log_content_ += current_time_and_date() + "\n";
                log_content_ += std::to_string(status_code) + "\n";
                log_content_ += headers_to_string(headers);
                if (!data.empty())
                {
                    log_content_ += "<<\n";
                    log_content_ += data + "\n";
                    log_content_ += ">>\n";
                }
                log_content_ += "\n";
                write_log();
            });
        }
    private:
        str data_location_;
        
        bool logging_enabled_;
        str log_content_;
        std::mutex mutex_;
        bool writing_ = false;
        
        void read_log()
        {
            file_readall(data_location_ + "/error.log", [this](arr<p1> content)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                
                log_content_ = str(content.begin(), content.end()) + log_content_;
            });
        }

        void write_log()
        {
            if (writing_)
                return;
            writing_ = true;
            
            auto content = arr<p1>();
            content.resize(log_content_.size());
            std::copy(log_content_.begin(), log_content_.end(), content.begin());
            file_writeall(data_location_ + "/error.log", content, [this, len = content.size()](bool success)
            {
                std::lock_guard<std::mutex> lock(mutex_);

                writing_ = false;
                if (!success)
                    return;
                if (len != log_content_.size())
                    write_log();
            });
        }
    };
}
