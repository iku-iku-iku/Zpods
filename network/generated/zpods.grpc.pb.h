// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: zpods.proto
// Original file comments:
// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef GRPC_zpods_2eproto__INCLUDED
#define GRPC_zpods_2eproto__INCLUDED

#include "zpods.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace zpods {

class FileService final {
 public:
  static constexpr char const* service_full_name() {
    return "zpods.FileService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    std::unique_ptr< ::grpc::ClientWriterInterface< ::zpods::FileChunk>> UploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response) {
      return std::unique_ptr< ::grpc::ClientWriterInterface< ::zpods::FileChunk>>(UploadFileRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::zpods::FileChunk>> AsyncUploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::zpods::FileChunk>>(AsyncUploadFileRaw(context, response, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::zpods::FileChunk>> PrepareAsyncUploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncWriterInterface< ::zpods::FileChunk>>(PrepareAsyncUploadFileRaw(context, response, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void UploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::ClientWriteReactor< ::zpods::FileChunk>* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientWriterInterface< ::zpods::FileChunk>* UploadFileRaw(::grpc::ClientContext* context, ::zpods::UploadStatus* response) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::zpods::FileChunk>* AsyncUploadFileRaw(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncWriterInterface< ::zpods::FileChunk>* PrepareAsyncUploadFileRaw(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    std::unique_ptr< ::grpc::ClientWriter< ::zpods::FileChunk>> UploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response) {
      return std::unique_ptr< ::grpc::ClientWriter< ::zpods::FileChunk>>(UploadFileRaw(context, response));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::zpods::FileChunk>> AsyncUploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::zpods::FileChunk>>(AsyncUploadFileRaw(context, response, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncWriter< ::zpods::FileChunk>> PrepareAsyncUploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncWriter< ::zpods::FileChunk>>(PrepareAsyncUploadFileRaw(context, response, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void UploadFile(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::ClientWriteReactor< ::zpods::FileChunk>* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientWriter< ::zpods::FileChunk>* UploadFileRaw(::grpc::ClientContext* context, ::zpods::UploadStatus* response) override;
    ::grpc::ClientAsyncWriter< ::zpods::FileChunk>* AsyncUploadFileRaw(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncWriter< ::zpods::FileChunk>* PrepareAsyncUploadFileRaw(::grpc::ClientContext* context, ::zpods::UploadStatus* response, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_UploadFile_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status UploadFile(::grpc::ServerContext* context, ::grpc::ServerReader< ::zpods::FileChunk>* reader, ::zpods::UploadStatus* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_UploadFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_UploadFile() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_UploadFile() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status UploadFile(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::zpods::FileChunk>* /*reader*/, ::zpods::UploadStatus* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestUploadFile(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::zpods::UploadStatus, ::zpods::FileChunk>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncClientStreaming(0, context, reader, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_UploadFile<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_UploadFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_UploadFile() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackClientStreamingHandler< ::zpods::FileChunk, ::zpods::UploadStatus>(
            [this](
                   ::grpc::CallbackServerContext* context, ::zpods::UploadStatus* response) { return this->UploadFile(context, response); }));
    }
    ~WithCallbackMethod_UploadFile() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status UploadFile(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::zpods::FileChunk>* /*reader*/, ::zpods::UploadStatus* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerReadReactor< ::zpods::FileChunk>* UploadFile(
      ::grpc::CallbackServerContext* /*context*/, ::zpods::UploadStatus* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_UploadFile<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_UploadFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_UploadFile() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_UploadFile() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status UploadFile(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::zpods::FileChunk>* /*reader*/, ::zpods::UploadStatus* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_UploadFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_UploadFile() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_UploadFile() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status UploadFile(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::zpods::FileChunk>* /*reader*/, ::zpods::UploadStatus* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestUploadFile(::grpc::ServerContext* context, ::grpc::ServerAsyncReader< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* reader, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncClientStreaming(0, context, reader, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_UploadFile : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_UploadFile() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackClientStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, ::grpc::ByteBuffer* response) { return this->UploadFile(context, response); }));
    }
    ~WithRawCallbackMethod_UploadFile() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status UploadFile(::grpc::ServerContext* /*context*/, ::grpc::ServerReader< ::zpods::FileChunk>* /*reader*/, ::zpods::UploadStatus* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerReadReactor< ::grpc::ByteBuffer>* UploadFile(
      ::grpc::CallbackServerContext* /*context*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  typedef Service StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef Service StreamedService;
};

class UserService final {
 public:
  static constexpr char const* service_full_name() {
    return "zpods.UserService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status Register(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::zpods::RegisterResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::RegisterResponse>> AsyncRegister(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::RegisterResponse>>(AsyncRegisterRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::RegisterResponse>> PrepareAsyncRegister(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::RegisterResponse>>(PrepareAsyncRegisterRaw(context, request, cq));
    }
    virtual ::grpc::Status Login(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::zpods::LoginResponse* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::LoginResponse>> AsyncLogin(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::LoginResponse>>(AsyncLoginRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::LoginResponse>> PrepareAsyncLogin(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::zpods::LoginResponse>>(PrepareAsyncLoginRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void Register(::grpc::ClientContext* context, const ::zpods::RegisterRequest* request, ::zpods::RegisterResponse* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Register(::grpc::ClientContext* context, const ::zpods::RegisterRequest* request, ::zpods::RegisterResponse* response, ::grpc::ClientUnaryReactor* reactor) = 0;
      virtual void Login(::grpc::ClientContext* context, const ::zpods::LoginRequest* request, ::zpods::LoginResponse* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Login(::grpc::ClientContext* context, const ::zpods::LoginRequest* request, ::zpods::LoginResponse* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::zpods::RegisterResponse>* AsyncRegisterRaw(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::zpods::RegisterResponse>* PrepareAsyncRegisterRaw(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::zpods::LoginResponse>* AsyncLoginRaw(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::zpods::LoginResponse>* PrepareAsyncLoginRaw(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status Register(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::zpods::RegisterResponse* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::RegisterResponse>> AsyncRegister(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::RegisterResponse>>(AsyncRegisterRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::RegisterResponse>> PrepareAsyncRegister(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::RegisterResponse>>(PrepareAsyncRegisterRaw(context, request, cq));
    }
    ::grpc::Status Login(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::zpods::LoginResponse* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::LoginResponse>> AsyncLogin(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::LoginResponse>>(AsyncLoginRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::LoginResponse>> PrepareAsyncLogin(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::zpods::LoginResponse>>(PrepareAsyncLoginRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void Register(::grpc::ClientContext* context, const ::zpods::RegisterRequest* request, ::zpods::RegisterResponse* response, std::function<void(::grpc::Status)>) override;
      void Register(::grpc::ClientContext* context, const ::zpods::RegisterRequest* request, ::zpods::RegisterResponse* response, ::grpc::ClientUnaryReactor* reactor) override;
      void Login(::grpc::ClientContext* context, const ::zpods::LoginRequest* request, ::zpods::LoginResponse* response, std::function<void(::grpc::Status)>) override;
      void Login(::grpc::ClientContext* context, const ::zpods::LoginRequest* request, ::zpods::LoginResponse* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::zpods::RegisterResponse>* AsyncRegisterRaw(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::zpods::RegisterResponse>* PrepareAsyncRegisterRaw(::grpc::ClientContext* context, const ::zpods::RegisterRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::zpods::LoginResponse>* AsyncLoginRaw(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::zpods::LoginResponse>* PrepareAsyncLoginRaw(::grpc::ClientContext* context, const ::zpods::LoginRequest& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Register_;
    const ::grpc::internal::RpcMethod rpcmethod_Login_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Register(::grpc::ServerContext* context, const ::zpods::RegisterRequest* request, ::zpods::RegisterResponse* response);
    virtual ::grpc::Status Login(::grpc::ServerContext* context, const ::zpods::LoginRequest* request, ::zpods::LoginResponse* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Register() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestRegister(::grpc::ServerContext* context, ::zpods::RegisterRequest* request, ::grpc::ServerAsyncResponseWriter< ::zpods::RegisterResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Login : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Login() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_Login() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Login(::grpc::ServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestLogin(::grpc::ServerContext* context, ::zpods::LoginRequest* request, ::grpc::ServerAsyncResponseWriter< ::zpods::LoginResponse>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Register<WithAsyncMethod_Login<Service > > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Register() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::zpods::RegisterRequest, ::zpods::RegisterResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::zpods::RegisterRequest* request, ::zpods::RegisterResponse* response) { return this->Register(context, request, response); }));}
    void SetMessageAllocatorFor_Register(
        ::grpc::MessageAllocator< ::zpods::RegisterRequest, ::zpods::RegisterResponse>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::zpods::RegisterRequest, ::zpods::RegisterResponse>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Register(
      ::grpc::CallbackServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithCallbackMethod_Login : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Login() {
      ::grpc::Service::MarkMethodCallback(1,
          new ::grpc::internal::CallbackUnaryHandler< ::zpods::LoginRequest, ::zpods::LoginResponse>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::zpods::LoginRequest* request, ::zpods::LoginResponse* response) { return this->Login(context, request, response); }));}
    void SetMessageAllocatorFor_Login(
        ::grpc::MessageAllocator< ::zpods::LoginRequest, ::zpods::LoginResponse>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(1);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::zpods::LoginRequest, ::zpods::LoginResponse>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Login() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Login(::grpc::ServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Login(
      ::grpc::CallbackServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_Register<WithCallbackMethod_Login<Service > > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Register() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Login : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Login() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_Login() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Login(::grpc::ServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Register() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestRegister(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_Login : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Login() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_Login() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Login(::grpc::ServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestLogin(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Register() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Register(context, request, response); }));
    }
    ~WithRawCallbackMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Register(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Login : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Login() {
      ::grpc::Service::MarkMethodRawCallback(1,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Login(context, request, response); }));
    }
    ~WithRawCallbackMethod_Login() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Login(::grpc::ServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Login(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Register() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::zpods::RegisterRequest, ::zpods::RegisterResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::zpods::RegisterRequest, ::zpods::RegisterResponse>* streamer) {
                       return this->StreamedRegister(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Register(::grpc::ServerContext* /*context*/, const ::zpods::RegisterRequest* /*request*/, ::zpods::RegisterResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedRegister(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::zpods::RegisterRequest,::zpods::RegisterResponse>* server_unary_streamer) = 0;
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Login : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Login() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::StreamedUnaryHandler<
          ::zpods::LoginRequest, ::zpods::LoginResponse>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::zpods::LoginRequest, ::zpods::LoginResponse>* streamer) {
                       return this->StreamedLogin(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Login() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Login(::grpc::ServerContext* /*context*/, const ::zpods::LoginRequest* /*request*/, ::zpods::LoginResponse* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedLogin(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::zpods::LoginRequest,::zpods::LoginResponse>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Register<WithStreamedUnaryMethod_Login<Service > > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_Register<WithStreamedUnaryMethod_Login<Service > > StreamedService;
};

}  // namespace zpods


#endif  // GRPC_zpods_2eproto__INCLUDED