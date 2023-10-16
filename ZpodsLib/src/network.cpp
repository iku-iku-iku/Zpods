//
// Created by code4love on 23-10-9.
//

#include "network.h"

auto zpods::User::to_json_str() const -> std::string {
    nlohmann::json json_obj;
    json_obj["id"] = id;
    json_obj["username"] = username;
    json_obj["password"] = password;
    return json_obj.dump();
}

auto zpods::User::request(const char *path) const {
    let_mut client = Client::connect_server();
    // 将 JSON 对象转换为字符串
    std::string json_str = to_json_str();
    auto &&res = client.post_with_json(path, json_str);
    dump_res(res, path);
    return res;
}

zpods::Status zpods::User::register_() const {
    let res = request("/user/register");

    // 处理响应
    if (res && res->status == 200) {
        if (res->body.find("already exists") != std::string::npos) {
            return Status::USER_ALREADY_EXISTS;
        }
        return Status::OK;
    }

    return Status::ERROR;
}

zpods::Status zpods::User::login() const {
    let res = request("/user/login");

    // 处理响应
    if (res && res->status == 200) {
        if (res->body.find("Wrong password") != std::string::npos) {
            return Status::WRONG_PASSWORD;
        }
        if (res->body.find("does not exist") != std::string::npos) {
            return Status::USER_NOT_EXISTS;
        }
        return Status::OK;
    }

    return Status::ERROR;
}

zpods::Status zpods::User::unregister() const {
    let res = request("/user/unregister");

    // 处理响应
    if (res && res->status == 200) {
        if (res->body.find("not exist") != std::string::npos) {
            return Status::USER_NOT_EXISTS;
        }
        return Status::OK;
    }

    return Status::ERROR;
}

zpods::Status zpods::User::upload_file(const char *file_path) {
    let_mut client = Client::connect_server();
    // 创建multipart表单
    let content = zpods::fs::read_from_file(file_path);
    let filename = zpods::fs::get_file_name(file_path);
    let hdfs_path = fmt::format("{}/{}", username, filename);
    httplib::MultipartFormDataItems items = {
            {"file", content, hdfs_path, "text/plain"},
    };

    let res = client.post_with_file("/file/upload", items);

    if (res && res->status == 200) {
        spdlog::info("{}", res->body);
    } else {
        if (res) {
            spdlog::info("Status: {}", res->status);
            spdlog::info("Body: {}", res->body);
            return Status::ERROR;
        }
    }
    return Status::OK;
}

zpods::Status zpods::User::download_file(const char *hdfs_path, const char *local_dir) {
    let_mut client = Client::connect_server();  // 替换为您的Spring Boot应用地址和端口
    let true_hdfs_path = fmt::format("{}/{}", username, hdfs_path);
    let path = fmt::format("/file/download?hdfsSource={}", true_hdfs_path);
    let local_path = fmt::format("{}/{}", local_dir, hdfs_path);
    auto res = client.get(path);

    if (res && res->status == 200) {
        // 保存文件或处理文件数据
        let_mut output_file = fs::open_or_create_file_as_ofs(local_path.c_str(), fs::ios::binary);
        spdlog::info("FILE SIZE: {}", res->body.size());
        output_file.write(res->body.data(), res->body.size());
        output_file.close();
    } else {
        spdlog::info("Failed to download file! HTTP Status: {}", res->status);
        return Status::ERROR;
    }

    return Status::OK;
}

zpods::Status zpods::upload_file() {
    httplib::Client client("localhost", 8080);  // 替换为您的Spring Boot应用地址和端口

    // 创建multipart表单
    let path = "/home/code4love/tmp/xxx.txt";
    let content = zpods::fs::read_from_file(path);
    httplib::Headers headers = {
            {"hdfsPath", "cpp.txt"}
    };
    httplib::MultipartFormDataItems items = {
            {"file", content, "user/itismygo", "text/plain"},
    };

    auto res = client.Post("/file/upload", items);

    if (res && res->status == 200) {
        std::cout << "File uploaded successfully!" << std::endl;
        std::cout << res->body << std::endl;
    } else {
        std::cout << "Failed to upload file!" << std::endl;
        if (res) {
            std::cout << "Status: " << res->status << std::endl;
            std::cout << "Body: " << res->body << std::endl;
        }
    }
    return Status::OK;
}

zpods::Status zpods::download_file(const char *hdfs_path, const char *local_path) {
    httplib::Client client("localhost", 8080);  // 更改为你的服务器地址和端口
    let path = fmt::format("/file/download?hdfsSource={}", hdfs_path);
    auto res = client.Get(path);

    if (res && res->status == 200) {
        // 保存文件或处理文件数据
        let_mut output_file = fs::open_or_create_file_as_ofs(local_path, fs::ios::binary);
        spdlog::info("FILE SIZE: {}", res->body.size());
        output_file.write(res->body.data(), res->body.size());
        output_file.close();
    } else {
        std::cout << "Failed to download file! HTTP Status: " << res->status << std::endl;
        return Status::ERROR;
    }

    return Status::OK;
}
