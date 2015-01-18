/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef ProxyService_H
#define ProxyService_H

#include <thrift/TDispatchProcessor.h>
#include "proxy_types.h"

namespace ProxyServer {

class ProxyServiceIf {
 public:
  virtual ~ProxyServiceIf() {}
  virtual void fetchWebPage(std::string& _return, const std::string& URL) = 0;
};

class ProxyServiceIfFactory {
 public:
  typedef ProxyServiceIf Handler;

  virtual ~ProxyServiceIfFactory() {}

  virtual ProxyServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(ProxyServiceIf* /* handler */) = 0;
};

class ProxyServiceIfSingletonFactory : virtual public ProxyServiceIfFactory {
 public:
  ProxyServiceIfSingletonFactory(const boost::shared_ptr<ProxyServiceIf>& iface) : iface_(iface) {}
  virtual ~ProxyServiceIfSingletonFactory() {}

  virtual ProxyServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(ProxyServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<ProxyServiceIf> iface_;
};

class ProxyServiceNull : virtual public ProxyServiceIf {
 public:
  virtual ~ProxyServiceNull() {}
  void fetchWebPage(std::string& /* _return */, const std::string& /* URL */) {
    return;
  }
};

typedef struct _ProxyService_fetchWebPage_args__isset {
  _ProxyService_fetchWebPage_args__isset() : URL(false) {}
  bool URL;
} _ProxyService_fetchWebPage_args__isset;

class ProxyService_fetchWebPage_args {
 public:

  ProxyService_fetchWebPage_args() : URL() {
  }

  virtual ~ProxyService_fetchWebPage_args() throw() {}

  std::string URL;

  _ProxyService_fetchWebPage_args__isset __isset;

  void __set_URL(const std::string& val) {
    URL = val;
  }

  bool operator == (const ProxyService_fetchWebPage_args & rhs) const
  {
    if (!(URL == rhs.URL))
      return false;
    return true;
  }
  bool operator != (const ProxyService_fetchWebPage_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ProxyService_fetchWebPage_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class ProxyService_fetchWebPage_pargs {
 public:


  virtual ~ProxyService_fetchWebPage_pargs() throw() {}

  const std::string* URL;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _ProxyService_fetchWebPage_result__isset {
  _ProxyService_fetchWebPage_result__isset() : success(false) {}
  bool success;
} _ProxyService_fetchWebPage_result__isset;

class ProxyService_fetchWebPage_result {
 public:

  ProxyService_fetchWebPage_result() : success() {
  }

  virtual ~ProxyService_fetchWebPage_result() throw() {}

  std::string success;

  _ProxyService_fetchWebPage_result__isset __isset;

  void __set_success(const std::string& val) {
    success = val;
  }

  bool operator == (const ProxyService_fetchWebPage_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const ProxyService_fetchWebPage_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ProxyService_fetchWebPage_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _ProxyService_fetchWebPage_presult__isset {
  _ProxyService_fetchWebPage_presult__isset() : success(false) {}
  bool success;
} _ProxyService_fetchWebPage_presult__isset;

class ProxyService_fetchWebPage_presult {
 public:


  virtual ~ProxyService_fetchWebPage_presult() throw() {}

  std::string* success;

  _ProxyService_fetchWebPage_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class ProxyServiceClient : virtual public ProxyServiceIf {
 public:
  ProxyServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  ProxyServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void fetchWebPage(std::string& _return, const std::string& URL);
  void send_fetchWebPage(const std::string& URL);
  void recv_fetchWebPage(std::string& _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class ProxyServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<ProxyServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (ProxyServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_fetchWebPage(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  ProxyServiceProcessor(boost::shared_ptr<ProxyServiceIf> iface) :
    iface_(iface) {
    processMap_["fetchWebPage"] = &ProxyServiceProcessor::process_fetchWebPage;
  }

  virtual ~ProxyServiceProcessor() {}
};

class ProxyServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  ProxyServiceProcessorFactory(const ::boost::shared_ptr< ProxyServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< ProxyServiceIfFactory > handlerFactory_;
};

class ProxyServiceMultiface : virtual public ProxyServiceIf {
 public:
  ProxyServiceMultiface(std::vector<boost::shared_ptr<ProxyServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~ProxyServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<ProxyServiceIf> > ifaces_;
  ProxyServiceMultiface() {}
  void add(boost::shared_ptr<ProxyServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void fetchWebPage(std::string& _return, const std::string& URL) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->fetchWebPage(_return, URL);
    }
    ifaces_[i]->fetchWebPage(_return, URL);
    return;
  }

};

} // namespace

#endif
