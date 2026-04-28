//
//  NCFileInfo-Wrapper.mm
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

#import <Foundation/Foundation.h>
#include <atomic>

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

@implementation TCInfo



- (id)initWithTimeUnits:(NSString *)timeUnits time_noleap:(bool)time_noleap timeInterval:(double)timeInterval firstTValue:(double)firstTValue {
    self = [super init];
    if (self) {
        self.timeUnits = timeUnits;
        self.time_noleap = time_noleap;
        self.timeInterval = timeInterval;
        self.firstTValue = firstTValue;
    }
    return self;
}

@end

@implementation TCs

- (id)initWithRealTCs:(NSMutableArray<Typhoon *>*)tcs tcInfo:(TCInfo*)tcInfo {
    self = [super init];
    if (self) {
        self.tcs = tcs;
        self.tcInfo = tcInfo;
    }
    return self;
}

@end

@interface NCFileInfo_Wrapper() {
    TTCore::NCFileInfo *m_instance;
    std::atomic_bool m_shouldCancel;
}


- (TCs*)copyToMangaged:(TTCore::TCs&)inTC;
@end

@implementation NCFileInfo_Wrapper

- (id)initWithNcFilePath:(NSString *)filePath {
    self = [super init];
    if (self) {
        m_instance = new TTCore::NCFileInfo([filePath cStringUsingEncoding:NSUTF8StringEncoding]);
        m_shouldCancel.store(false);
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
        m_shouldCancel.store(false);
    }
    return self;
}

- (id)initWithNcFilePath:(NSString *)filePath :(bool)isWrfoutFile :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor :(int)zLevelIndex :(int)threadNum :(double)toGridRes :(NSString *)tempFileDir {
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
        
        m_instance = new TTCore::NCFileInfo(filePathStr, isWrfoutFile, varNames, zLevelIndex, toGridRes, true, threadNum, tempFileDirStr, resourcePathStr);
        m_shouldCancel.store(false);
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

- (void)resetCancelState {
    m_shouldCancel.store(false);
}

- (void)requestCancel {
    m_shouldCancel.store(true);
}

- (bool)wasCancelled {
    return m_shouldCancel.load();
}

- (TCs*)startTrackingWithStepPgCallback : (void(*)(int stepIdx, void*))stepPgCallback andWith :(void(*)(double progressValue, void*)) progressCallback withTarget: (void*) target {
    TTCore::TCs tcs;

    m_instance->startTracking(tcs, stepPgCallback, progressCallback, target, &m_shouldCancel);
    
    std::cout << "unmanaged TC number: " << tcs.size() << std::endl;
    
//    NSMutableArray<Typhoon *> *outTC = [NSMutableArray new];
//    [self copyToMangaged:tcs];
    return [self copyToMangaged:tcs];
    
}

- (TCs*)copyToMangaged:(TTCore::TCs&)inTC {
    auto realTCs = inTC.getTcs();
    int tcSize = inTC.size();
    NSMutableArray<Typhoon *> *outTC = [NSMutableArray new];
    
    for (int i = 0; i < tcSize; ++i) {
        NSMutableArray<YXIndex *> *maxVorCells = [NSMutableArray new];
        for (const auto &maxVorCell : realTCs[i].maxVorCells) {
            [maxVorCells addObject:[[YXIndex alloc] initWithYIndex:maxVorCell.first xIndex:maxVorCell.second]];
        }
        NSMutableArray<LatLon *> *geoCenters = [NSMutableArray new];
        for (const auto &geoCenter : realTCs[i].geoCenters) {
            [geoCenters addObject:[[LatLon alloc] initWithLat:geoCenter.first lon:geoCenter.second]];
        }
        
        Typhoon *newTC = [Typhoon new];
        newTC.serialNo = realTCs[i].serialNo;
        newTC.maxVorCells = maxVorCells;
        newTC.geoCenters = geoCenters;
        newTC.startTimeIndex = realTCs[i].startTimeIndex;
        newTC.endTimeIndex = realTCs[i].endTimeIndex;
        
        [outTC addObject:newTC];
    }
    
    TCInfo *newTCInfo = [[TCInfo alloc] initWithTimeUnits:[NSString stringWithUTF8String:inTC.getTimeUnits().c_str()] time_noleap:inTC.getTcInfo().getTime_noleap() timeInterval:inTC.getTimeInterval() firstTValue:inTC.getTcInfo().getFirstTValue()];
    
    return [[TCs alloc] initWithRealTCs:outTC tcInfo:newTCInfo];
    
}

- (void)exportFile_json:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath {
    TTCore::TCs tcs_cpp = [self toCppTCs:tcs];
    m_instance->exportFile_json(tcs_cpp, [oNcFilePath cStringUsingEncoding:NSUTF8StringEncoding]);
}

- (void)exportFile_proto3:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath {
    TTCore::TCs tcs_cpp = [self toCppTCs:tcs];
    m_instance->exportFile_proto3(tcs_cpp, [oNcFilePath cStringUsingEncoding:NSUTF8StringEncoding]);
}

- (void)exportFile_nc:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath fullCommand:(NSString*)fullCommand {
    
    TTCore::TCs tcs_cpp = [self toCppTCs:tcs];
    
    m_instance->exportFile_nc(tcs_cpp, [oNcFilePath cStringUsingEncoding:NSUTF8StringEncoding], [fullCommand cStringUsingEncoding:NSUTF8StringEncoding]);
}

- (void)exportFile_nc_compact:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath fullCommand:(NSString*)fullCommand {
    TTCore::TCs tcs_cpp = [self toCppTCs:tcs];
    
    m_instance->exportFile_nc_compact(tcs_cpp, [oNcFilePath cStringUsingEncoding:NSUTF8StringEncoding], [fullCommand cStringUsingEncoding:NSUTF8StringEncoding]);
    
}

- (TTCore::TCs)toCppTCs:(TCs*)tcs {
    std::vector<TTCore::Typhoon> realTCs;

    for (Typhoon *realTC in tcs.tcs) {
        std::vector<std::pair<int, int>> maxVorCells_cpp;
        std::vector<std::pair<float, float>> geoCenters_cpp;
        for (YXIndex *maxVorCell in realTC.maxVorCells) {
            maxVorCells_cpp.push_back({maxVorCell.yIndex, maxVorCell.xIndex});
        }
        for (LatLon *geoCenter in realTC.geoCenters) {
            geoCenters_cpp.push_back({geoCenter.lat, geoCenter.lon});
        }
        
        realTCs.push_back(TTCore::Typhoon{realTC.serialNo, maxVorCells_cpp, geoCenters_cpp, realTC.startTimeIndex, realTC.endTimeIndex, {}});
    }
    
    auto tcInfo = TTCore::TCInfo([tcs.tcInfo.timeUnits cStringUsingEncoding:NSUTF8StringEncoding], tcs.tcInfo.time_noleap, tcs.tcInfo.timeInterval, tcs.tcInfo.firstTValue);
    
    auto tcs_cpp = TTCore::TCs(realTCs, tcInfo);
    
    return tcs_cpp;
}

@end

