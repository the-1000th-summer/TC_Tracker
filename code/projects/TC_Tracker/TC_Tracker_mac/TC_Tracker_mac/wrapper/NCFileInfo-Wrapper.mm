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

- (id)initWithNcFilePath:(NSString *)filePath {
    self = [super init];
    if (self) {
        m_instance = new TTCore::NCFileInfo([filePath cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    return self;
}

- (void)dealloc {
//    m_instance = nil;
    delete m_instance;
//    [super dealloc];       // no need to call [super dealloc] when using ARC
}

- (bool)checkFileValid:(NSString **)fileValidInfo {
    m_instance->checkFileValid();
    *fileValidInfo = [NSString stringWithUTF8String:m_instance->fileValidInfo.c_str()];
    return m_instance->isFileValid;
}

- (NSMutableArray *)getVarsName {
    std::vector<std::string> varsName = m_instance->getVarsName();
    NSMutableArray *nsstrings = [NSMutableArray new];
    for (auto const &varName : varsName) {
        id nsstr = [NSString stringWithUTF8String:varName.c_str()];
        [nsstrings addObject:nsstr];
    }
    return nsstrings;
}

@end

