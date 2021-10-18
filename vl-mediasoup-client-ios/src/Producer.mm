//
//  Producer.mm
//  mediasoup-client-ios
//
//  Created by Ethan.
//  Copyright © 2019 Ethan. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WebRTC/RTCPeerConnectionFactory.h>
#import <WebRTC/RTCMediaStreamTrack.h>
#import "Producer.h"
#import "ProducerWrapper.h"

@interface Producer ()
@property(nonatomic, retain) RTCPeerConnectionFactory *factory;
@end

@implementation Producer : NSObject

-(instancetype)initWithNativeProducer:(NSValue *)nativeProducer {
    self = [super init];
    if (self) {
        __nativeProducer = [nativeProducer retain];
        
        webrtc::MediaStreamTrackInterface *nativeTrack = [ProducerWrapper getNativeTrack:self._nativeProducer];
        rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track(nativeTrack);

        _factory = [[RTCPeerConnectionFactory alloc] init];
        __nativeTrack = [[RTCMediaStreamTrack mediaTrackForNativeTrack:track factory:_factory] retain];
    }
    
    return self;
}

-(void)dealloc {
    if (__nativeProducer != nil && ![ProducerWrapper isNativeClosed:__nativeProducer]) {
        [ProducerWrapper nativeClose:__nativeProducer];
    }
    [__nativeProducer release];
    [__nativeTrack release];
    [_factory release];
    [super dealloc];
}

-(NSString *)getId {
    return [ProducerWrapper getNativeId:self._nativeProducer];
}

-(bool)isClosed {
    return [ProducerWrapper isNativeClosed:self._nativeProducer];
}

-(NSString *)getKind {
    return [ProducerWrapper getNativeKind:self._nativeProducer];
}

-(RTCMediaStreamTrack *)getTrack {
    return self._nativeTrack;
}

-(bool)isPaused {
    return [ProducerWrapper isNativePaused:self._nativeProducer];
}

-(int)getMaxSpatialLayer {
    return [ProducerWrapper getNativeMaxSpatialLayer:self._nativeProducer];
}

-(NSString *)getAppData {
    return [ProducerWrapper getNativeAppData:self._nativeProducer];
}

-(NSString *)getRtpParameters {
    return [ProducerWrapper getNativeRtpParameters:self._nativeProducer];
}

-(void)resume {
    [ProducerWrapper nativeResume:self._nativeProducer];
}

-(void)pause {
    [ProducerWrapper nativePause:self._nativeProducer];
}

-(void)setMaxSpatialLayers:(int)layer {
    [ProducerWrapper setNativeMaxSpatialLayer:self._nativeProducer layer:layer];
}

-(void)replaceTrack:(RTCMediaStreamTrack *)track {
    [ProducerWrapper nativeReplaceTrack:self._nativeProducer track:track.hash];
    
    webrtc::MediaStreamTrackInterface *nativeTrack = [ProducerWrapper getNativeTrack:self._nativeProducer];
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaTrack(nativeTrack);

    self._nativeTrack = [RTCMediaStreamTrack mediaTrackForNativeTrack:mediaTrack factory:[[RTCPeerConnectionFactory alloc] init]];
}

-(NSString *)getStats {
    return [ProducerWrapper getNativeStats:self._nativeProducer];
}

-(void)close {
    [ProducerWrapper nativeClose:self._nativeProducer];
}

@end