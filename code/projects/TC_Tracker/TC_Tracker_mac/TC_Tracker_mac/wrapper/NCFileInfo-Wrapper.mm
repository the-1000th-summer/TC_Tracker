//
//  NCFileInfo-Wrapper.mm
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

#import <Foundation/Foundation.h>

#import "NCFileInfo-Wrapper.h"
#import "NCFileInfo.h"
#include "Typhoon.h"

@interface YXIndex()
@property (nonatomic, readwrite) int yIndex;
@property (nonatomic, readwrite) int xIndex;
@end

@implementation YXIndex

- (id)initWithYIndex:(int)yIndex xIndex:(int)xIndex {
    self = [super init];
    if (self) {
        self.yIndex = yIndex;
        self.xIndex = xIndex;
    }
    return self;
}

@end

@interface LatLon()
@property (nonatomic, readwrite) float lat;
@property (nonatomic, readwrite) float lon;
@end

@implementation LatLon
- (id)initWithLat:(float)lat lon:(float)lon {
    self = [super init];
    if (self) {
        self.lat = lat;
        self.lon = lon;
    }
    return self;
}
@end

@implementation Typhoon

//- (id) initWithSerialNo:(int)serialNo :(NSMutableArray<LatLon *>*)maxVorCells {
//    self = [super init];
//    if (self) {
//        self->serialNo = serialNo;
//        self->maxVorCells = maxVorCells;
//    }
//    return self;
//}

@end

@interface NCFileInfo_Wrapper() {
    TTCore::NCFileInfo *m_instance;
}


- (void)copyToMangaged:(TTCore::TCs&)inTC :(NSMutableArray<Typhoon *>*) outTC;
@end

@implementation NCFileInfo_Wrapper

- (id)initWithNcFilePath:(NSString *)filePath {
    self = [super init];
    if (self) {
        m_instance = new TTCore::NCFileInfo([filePath cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    return self;
}

- (id)initWithNcFilePath:(NSString *)filePath :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor {
    self = [super init];
    if (self) {
        auto filePathStr = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
        auto timeStr = [time cStringUsingEncoding:NSUTF8StringEncoding];
        auto latStr = [lat cStringUsingEncoding:NSUTF8StringEncoding];
        auto lonStr = [lon cStringUsingEncoding:NSUTF8StringEncoding];
        auto vorStr = [vor cStringUsingEncoding:NSUTF8StringEncoding];
        auto uStr = [u cStringUsingEncoding:NSUTF8StringEncoding];
        auto vStr = [v cStringUsingEncoding:NSUTF8StringEncoding];
        
        auto varNames = VarNames(timeStr, latStr, lonStr, vorStr, uStr, vStr, dataIsVor);
        
        m_instance = new TTCore::NCFileInfo(filePathStr, varNames);
    }
    return self;
}

- (id)initWithNcFilePath:(NSString *)filePath :(bool)isWrfoutFile :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor :(int)zLevelIndex :(double)toGridRes :(NSString *)tempFileDir {
    self = [super init];
    if (self) {
        auto filePathStr = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
        auto timeStr = [time cStringUsingEncoding:NSUTF8StringEncoding];
        auto latStr = [lat cStringUsingEncoding:NSUTF8StringEncoding];
        auto lonStr = [lon cStringUsingEncoding:NSUTF8StringEncoding];
        auto vorStr = [vor cStringUsingEncoding:NSUTF8StringEncoding];
        auto uStr = [u cStringUsingEncoding:NSUTF8StringEncoding];
        auto vStr = [v cStringUsingEncoding:NSUTF8StringEncoding];
        auto tempFileDirStr = [tempFileDir cStringUsingEncoding:NSUTF8StringEncoding];
        
        auto varNames = VarNames(timeStr, latStr, lonStr, vorStr, uStr, vStr, dataIsVor);
        
        NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
        auto resourcePathStr = [resourcePath cStringUsingEncoding:NSUTF8StringEncoding];
        
        m_instance = new TTCore::NCFileInfo(filePathStr, isWrfoutFile, varNames, zLevelIndex, toGridRes, true, 1, tempFileDirStr, resourcePathStr);
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

- (int)getZLvDimLenName:(NSString **)zLvDimName; {
    std::string dimName = "";
    int zLvDimLen = m_instance->getZLvDimLenName(dimName);
    *zLvDimName = [NSString stringWithUTF8String:dimName.c_str()];
    return zLvDimLen;
}

- (NSMutableArray<Typhoon *>*)startTrackingWithStepPgCallback : (void(*)(int stepIdx, void*))stepPgCallback andWith :(void(*)(double progressValue, void*)) progressCallback withTarget: (void*) target {
    TTCore::TCs tcs;
    bool isCanceled = false;
    m_instance->startTracking(tcs, &isCanceled, stepPgCallback, progressCallback, target);
    
    std::cout << "unmanaged TC number: " << tcs.size() << std::endl;
    
    NSMutableArray<Typhoon *> *outTC = [NSMutableArray new];
    [self copyToMangaged:tcs :outTC];
    return outTC;
    
}

- (void)copyToMangaged:(TTCore::TCs&)inTC:(NSMutableArray<Typhoon *>*) outTC {
    auto tcs = inTC.getTcs();
    int tcSize = inTC.size();
    
    for (int i = 0; i < tcSize; ++i) {
        NSMutableArray<YXIndex *> *maxVorCells = [NSMutableArray new];
        for (const auto &maxVorCell : tcs[i].maxVorCells) {
            [maxVorCells addObject:[[YXIndex alloc] initWithYIndex:maxVorCell.first xIndex:maxVorCell.second]];
        }
        NSMutableArray<LatLon *> *geoCenters = [NSMutableArray new];
        for (const auto &geoCenter : tcs[i].geoCenters) {
            [geoCenters addObject:[[LatLon alloc] initWithLat:geoCenter.first lon:geoCenter.second]];
        }
        
        Typhoon *newTC = [Typhoon new];
        newTC.serialNo = tcs[i].serialNo;
        newTC.maxVorCells = maxVorCells;
        newTC.geoCenters = geoCenters;
        newTC.startTimeIndex = tcs[i].startTimeIndex;
        newTC.endTimeIndex = tcs[i].endTimeIndex;
        
        [outTC addObject:newTC];
    }
}

@end

