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

- (id)initWithNcFilePath:(NSString *)filePath :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v {
    self = [super init];
    if (self) {
        auto filePathStr = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
        auto timeStr = [time cStringUsingEncoding:NSUTF8StringEncoding];
        auto latStr = [lat cStringUsingEncoding:NSUTF8StringEncoding];
        auto lonStr = [lon cStringUsingEncoding:NSUTF8StringEncoding];
        auto vorStr = [vor cStringUsingEncoding:NSUTF8StringEncoding];
        auto uStr = [u cStringUsingEncoding:NSUTF8StringEncoding];
        auto vStr = [v cStringUsingEncoding:NSUTF8StringEncoding];
        
        auto varNames = VarNames(timeStr, latStr, lonStr, vorStr, uStr, vStr);
        
        m_instance = new TTCore::NCFileInfo(filePathStr, varNames);
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

- (NSMutableArray *)getVorDimsName:(NSString *)vorVarName {
    std::vector<std::string> dimsName = m_instance->getVorDimsName([vorVarName cStringUsingEncoding:NSUTF8StringEncoding]);
    NSMutableArray *nsstrings = [NSMutableArray new];
    for (auto const &dimName : dimsName) {
        id nsstr = [NSString stringWithUTF8String:dimName.c_str()];
        [nsstrings addObject:nsstr];
    }
    return nsstrings;
}

- (bool)checkIfIsWrfoutFile:(NSString **)exceptionInfo {
    std::string eInfo = "";
    bool isWrfoutFile = m_instance->checkIfIsWrfoutFile(eInfo);
    *exceptionInfo = [NSString stringWithUTF8String:eInfo.c_str()];
    return isWrfoutFile;
}

- (int)getZLvDimLenName: (NSString **)zLvDimName {
    std::string dimName = "";
    int zLvDimLen = m_instance->getZLvDimLenName(dimName);
    *zLvDimName = [NSString stringWithUTF8String:dimName.c_str()];
    return zLvDimLen;
}

@end

