//
//  RecvTransport.mm
//  mediasoup-client-ios
//
//  Created by Ethan.
//  Copyright © 2019 Ethan. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RecvTransport.h"
#import "TransportWrapper.h"
#import "Consumer.h"

@implementation RecvTransport : Transport

-(void)dealloc {
    [TransportWrapper nativeFreeRecvTransport:self._nativeTransport];
    self._nativeTransport = nil;
}

-(Consumer *)consume:(id<ConsumerListener>)listener id:(NSString *)id producerId:(NSString *)producerId kind:(NSString *)kind rtpParameters:(NSString *)rtpParameters {
    return [self consume:listener id:id producerId:producerId kind:kind rtpParameters:rtpParameters appData:nil];
}

-(Consumer *)consume:(id<ConsumerListener>)listener id:(NSString *)id producerId:(NSString *)producerId kind:(NSString *)kind rtpParameters:(NSString *)rtpParameters appData:(NSString *)appData {
    [self checkTransportExists];
    
    @synchronized(self) {
        @try {
            Consumer *consumer = [TransportWrapper nativeConsume:self._nativeTransport listener:listener id:id producerId:producerId kind:kind rtpParameters:rtpParameters appData:appData];

            return consumer;
        } @catch (...) {
            return nil;
        }
    }
}

-(void)checkTransportExists {
    if (self._nativeTransport == nil) {
        NSException* exception = [NSException exceptionWithName:@"IllegalStateException" reason:@"RecvTransport has been disposed." userInfo:nil];
        
        throw exception;
    }
}

@end
