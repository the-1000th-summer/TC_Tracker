//
//  NCFileInfo-Wrapper.h
//  TC_Tracker_mac
//
//  Created by 邓浩 on 2022/6/25.
//

#import <Foundation/Foundation.h>
//#include "NCFileInfo.h"

#ifndef NCFileInfo_Wrapper_h
#define NCFileInfo_Wrapper_h

@interface YXIndex : NSObject

@property (nonatomic, readonly) int yIndex;
@property (nonatomic, readonly) int xIndex;
- (id)initWithYIndex:(int)yIndex xIndex:(int)xIndex;

@end

@interface LatLon : NSObject

@property (nonatomic, readonly) float lat;
@property (nonatomic, readonly) float lon;
- (id)initWithLat:(float)lat lon:(float)lon;

@end

@interface Typhoon : NSObject

@property (nonatomic, readwrite) int serialNo;
@property (nonatomic, readwrite) NSMutableArray<YXIndex *> *maxVorCells;
@property (nonatomic, readwrite) NSMutableArray<LatLon *> *geoCenters;
@property (nonatomic, readwrite) int startTimeIndex;
@property (nonatomic, readwrite) int endTimeIndex;

//- (id) initWithSerialNo:(int)serialNo :(NSMutableArray<LatLon *>*)maxVorCells;

@end

@interface TCInfo : NSObject

@property (nonatomic, readwrite) NSString *timeUnits;
@property (nonatomic, readwrite) bool time_noleap;
@property (nonatomic, readwrite) double timeInterval;
/// 文件中time variable第一个数据的值
@property (nonatomic, readwrite) double firstTValue;

- (id)initWithTimeUnits:(NSString *)timeUnits time_noleap:(bool)time_noleap timeInterval:(double)timeInterval firstTValue:(double)firstTValue;

@end

@interface TCs: NSObject

@property (nonatomic, readwrite) NSMutableArray<Typhoon *>* tcs;
@property (nonatomic, readwrite) TCInfo* tcInfo;

- (id)initWithRealTCs:(NSMutableArray<Typhoon *>*)tcs tcInfo:(TCInfo*)tcInfo;

@end

@interface NCFileInfo_Wrapper : NSObject

// if do not want default init method, uncomment next line
//- (instancetype)init NS_UNAVAILABLE;
- (id)initWithNcFilePath:(NSString *)filePath;
- (id)initWithNcFilePath:(NSString *)filePath :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor;
- (id)initWithNcFilePath:(NSString *)filePath :(bool)isWrfoutFile :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor :(int)zLevelIndex :(int)threadNum :(double)toGridRes :(NSString *)tempFileDir;
- (void)dealloc;

- (bool)checkFileValid:(NSString **)fileValidInfo;
- (NSMutableArray *)getVarsName;
- (NSMutableArray *)getVorDimsName:(NSString *)vorVarName;
- (bool)checkIfIsWrfoutFile:(NSString **)exceptionInfo;
- (int)getZLvDimLenName:(NSString **)zLvDimName;

- (TCs*)startTrackingWithStepPgCallback : (void(*)(int stepIdx, void*))stepPgCallback andWith :(void(*)(double progressValue, void*)) progressCallback withTarget: (void*) target withCancelFlag: (bool*)shouldCancel;

- (void)exportFile_json:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath;
- (void)exportFile_proto3:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath;
- (void)exportFile_nc:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath fullCommand:(NSString*)fullCommand;
- (void)exportFile_nc_compact:(TCs*)tcs oNcFilePath:(NSString*)oNcFilePath fullCommand:(NSString*)fullCommand;

@end

#endif /* NCFileInfo_Wrapper_h */
