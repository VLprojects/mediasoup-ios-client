//
//  TransportWrapper.h
//  mediasoup-client-ios
//
//  Created by Ethan.
//  Copyright © 2019 Ethan. All rights reserved.
//

#import <libmediasoupclient/include/MediaSoupClientErrors.hpp>
#import "Transport.hpp"
#import "ProducerWrapper.h"
#import "ConsumerWrapper.h"
#import "SendTransport.h"
#import "RecvTransport.h"
#import "Transport.h"

#ifndef TransportWrapper_h
#define TransportWrapper_h

@class RTCPeerConnectionFactory;

@interface TransportWrapper : NSObject {}
+(NSString *)getNativeId:(NSValue *)nativeTransport;
+(NSString *)getNativeConnectionState:(NSValue *)nativeTransport;
+(NSString *)getNativeAppData:(NSValue *)nativeTransport;
+(NSString *)getNativeStats:(NSValue *)nativeTransport;
+(bool)isNativeClosed:(NSValue *)nativeTransport;
+(void)nativeRestartIce:(NSValue *)nativeTransport iceParameters:(NSString *)iceParameters;
+(void)nativeUpdateIceServers:(NSValue *)nativeTransport iceServers:(NSString *)iceServers;
+(void)nativeUpdateIceTransportPolicy:(NSValue *)nativeTransport policy:(RTCIceTransportPolicy)policy;
+(void)nativeClose:(NSValue *)nativeTransport;
+(NSValue *)nativeGetNativeTransport:(NSValue *)nativeTransport;
+(void)nativeFreeSendTransport:(NSValue *)nativeTransport;
+(void)nativeFreeRecvTransport:(NSValue *)nativeTransport;

+(::Producer *)nativeProduce:(NSValue *)nativeTransport listener:(Protocol<ProducerListener> *)listener
    pcFactory:(RTCPeerConnectionFactory *)pcFactory track:(NSUInteger)track
    encodings:(NSArray *)encodings codecOptions:(NSString *)codecOptions appData:(NSString *)appData;

+(::Consumer *)nativeConsume:(NSValue *)nativeTransport listener:(id<ConsumerListener>)listener
    pcFactory:(RTCPeerConnectionFactory *)pcFactory id:(NSString *)id
    producerId:(NSString *)producerId kind:(NSString *)kind rtpParameters:(NSString *)rtpParameters
    appData:(NSString *)appData;

+(mediasoupclient::Transport *)extractNativeTransport:(NSValue *)nativeTransport;

@end

class SendTransportListenerWrapper : public mediasoupclient::SendTransport::Listener {
private:
    __weak id<SendTransportListener> listener_;
public:
    SendTransportListenerWrapper(Protocol<SendTransportListener>* listener)
    : listener_(listener) {}
    
    ~SendTransportListenerWrapper() {
    }
    
    std::future<void> OnConnect(mediasoupclient::Transport* nativeTransport, const nlohmann::json& dtlsParameters) override {
        const std::string dtlsParametersString = dtlsParameters.dump();
        const std::string nativeId = nativeTransport->GetId();
        NSString* transportId = [NSString stringWithUTF8String:nativeId.c_str()];

        [this->listener_ onConnect:transportId dtlsParameters:[NSString stringWithUTF8String:dtlsParametersString.c_str()]];

        std::promise<void> promise;
        promise.set_value();

        return promise.get_future();
    };
    
    void OnConnectionStateChange(mediasoupclient::Transport* nativeTransport, const std::string& connectionState) override {
        const std::string nativeId = nativeTransport->GetId();
        NSString* transportId = [NSString stringWithUTF8String:nativeId.c_str()];

        [this->listener_ onConnectionStateChange:transportId connectionState:[NSString stringWithUTF8String:connectionState.c_str()]];
    };
    
    std::future<std::string> OnProduce(
                                       mediasoupclient::SendTransport* nativeTransport,
                                       const std::string& kind,
                                       nlohmann::json rtpParameters,
                                       const nlohmann::json& appData) override {
        
        const std::string rtpParametersString = rtpParameters.dump();
        const std::string appDataString = appData.dump();
        const std::string nativeId = nativeTransport->GetId();
        NSString* transportId = [NSString stringWithUTF8String:nativeId.c_str()];

        __block std::promise<std::string> promise;
        
        [this->listener_ onProduce:transportId
            kind: [NSString stringWithUTF8String: kind.c_str()]
            rtpParameters: [NSString stringWithUTF8String: rtpParametersString.c_str()]
            appData: [NSString stringWithUTF8String:appDataString.c_str()]
            callback: ^(NSString* id) {
                try {
                    if (id == nil) {
                        auto ep = make_exception_ptr(MediaSoupClientError("TransportIdIsNil"));
                        promise.set_exception(ep);
                    } else {
                        promise.set_value(std::string([id UTF8String]));
                    }
                } catch(...) {
                }
            }
         ];

        return promise.get_future();
    };
  
    std::future<std::string> OnProduceData(
                                           mediasoupclient::SendTransport* nativeTransport,
                                           const nlohmann::json& sctpStreamParameters,
                                           const std::string& label,
                                           const std::string& protocol,
                                           const nlohmann::json& appData) {
      
      __block std::promise<std::string> promise;
      promise.set_value(std::string("not implemented"));
      
      return promise.get_future();
    };
};

class OwnedTransport {
public:
    virtual mediasoupclient::Transport *transport() const = 0;
};

class OwnedSendTransport final: public OwnedTransport {
public:
    OwnedSendTransport(mediasoupclient::SendTransport* transport, SendTransportListenerWrapper* listener)
    : transport_(transport), listener_(listener) {

        NSLog(@"OwnedSendTransport constructor");
    }

    ~OwnedSendTransport() {
        NSLog(@"OwnedSendTransport destructor");

        delete listener_;
        delete transport_;
    };

    mediasoupclient::SendTransport *transport() const { return transport_; }

private:
    mediasoupclient::SendTransport* transport_;
    SendTransportListenerWrapper *listener_;
};

class RecvTransportListenerWrapper final : public mediasoupclient::RecvTransport::Listener {
private:
    __weak id<TransportListener> listener_;
public:
    RecvTransportListenerWrapper(Protocol<TransportListener>* listener)
    : listener_(listener) {}
    
    ~RecvTransportListenerWrapper() {
    }

    std::future<void> OnConnect(mediasoupclient::Transport* nativeTransport, const nlohmann::json& dtlsParameters) override {
        const std::string dtlsParametersString = dtlsParameters.dump();
        const std::string nativeId = nativeTransport->GetId();
        NSString* transportId = [NSString stringWithUTF8String:nativeId.c_str()];

        [this->listener_ onConnect:transportId dtlsParameters:[NSString stringWithUTF8String:dtlsParametersString.c_str()]];
        
        std::promise<void> promise;
        promise.set_value();

        return promise.get_future();
    };
    
    void OnConnectionStateChange(mediasoupclient::Transport* nativeTransport, const std::string& connectionState) override {
        const std::string nativeId = nativeTransport->GetId();
        NSString* transportId = [NSString stringWithUTF8String:nativeId.c_str()];

        [this->listener_ onConnectionStateChange:transportId connectionState:[NSString stringWithUTF8String:connectionState.c_str()]];
    };
};

class OwnedRecvTransport final : public OwnedTransport {
public:
    OwnedRecvTransport(mediasoupclient::RecvTransport* transport, RecvTransportListenerWrapper* listener)
    : transport_(transport), listener_(listener) {

        NSLog(@"OwnedRecvTransport constructor");
    }

    ~OwnedRecvTransport() {
        NSLog(@"OwnedRecvTransport destructor");

        delete listener_;
        delete transport_;
    };

    mediasoupclient::RecvTransport *transport() const { return transport_; }

private:
    mediasoupclient::RecvTransport* transport_;
    RecvTransportListenerWrapper *listener_;
};

#endif /* TransportWrapper_h */
