//
//  NCFileInfo-Wrapper.mm
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

#import <Foundation/Foundation.h>

#import "NCFileInfo-Wrapper.h"
#import "NCFileInfo.h"

@interface NCFileInfo_Wrapper() {
    TTCore::NCFileInfo *m_instance;
}
@end

@implementation NCFileInfo_Wrapper

- (id)initWithArgs:(NSString *)filePath {
    m_instance->ncFilePath = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
    return self;
}

- (void)dealloc {
//    m_instance = nil;
}

- (void)checkFileValid:(NSString *)fileValidInfo {
    
}

@end

