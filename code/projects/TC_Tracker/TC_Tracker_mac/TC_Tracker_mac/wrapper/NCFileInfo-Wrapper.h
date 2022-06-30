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

@interface LatLon : NSObject {
    float lat;
    float lon;
}
@end

@interface Typhoon : NSObject {
    int serialNo;
    NSMutableArray<LatLon *> *maxVorCells;
    NSMutableArray<LatLon *> *geoCenters;
    int startTimeIndex;
    int endTimeIndex;
}


@end

@interface NCFileInfo_Wrapper : NSObject


- (instancetype)init NS_UNAVAILABLE;
- (id)initWithNcFilePath:(NSString *)filePath;
- (id)initWithNcFilePath:(NSString *)filePath :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v;
- (id)initWithNcFilePath:(NSString *)filePath :(bool)isWrfoutFile :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v :(int)zLevelIndex :(NSString *)tempFileDir;
- (void)dealloc;

- (bool)checkFileValid:(NSString **)fileValidInfo;
- (NSMutableArray *)getVarsName;
- (NSMutableArray *)getVorDimsName:(NSString *)vorVarName;
- (bool)checkIfIsWrfoutFile:(NSString **)exceptionInfo;
- (int)getZLvDimLenName:(NSString *)theVarName :(NSString **)zLvDimName;

- (void)startTracking;

@end

#endif /* NCFileInfo_Wrapper_h */
