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

@interface NCFileInfo_Wrapper : NSObject


- (instancetype)init NS_UNAVAILABLE;
- (id)initWithNcFilePath:(NSString *)filePath;
- (id)initWithNcFilePath:(NSString *)filePath :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor;
- (id)initWithNcFilePath:(NSString *)filePath :(bool)isWrfoutFile :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(bool)dataIsVor :(int)zLevelIndex :(NSString *)tempFileDir;
- (void)dealloc;

- (bool)checkFileValid:(NSString **)fileValidInfo;
- (NSMutableArray *)getVarsName;
- (NSMutableArray *)getVorDimsName:(NSString *)vorVarName;
- (bool)checkIfIsWrfoutFile:(NSString **)exceptionInfo;
- (int)getZLvDimLenName:(NSString **)zLvDimName;

- (NSMutableArray<Typhoon *>*)startTrackingWithCallback :(void(*)(void*)) progressCallback andWith: (void(*)(bool result, void* target)) resultCallback withTarget: (void*) target;


@end

#endif /* NCFileInfo_Wrapper_h */
