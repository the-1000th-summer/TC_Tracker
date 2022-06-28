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

@interface NCFileInfo_Wrapper : NSObject


- (instancetype)init NS_UNAVAILABLE;
- (id)initWithNcFilePath:(NSString *)filePath;
- (id)initWithNcFilePath:(NSString *)filePath :(NSString *)time :(NSString *)lat :(NSString *)lon :(NSString *)vor :(NSString *)u :(NSString *)v;
- (void)dealloc;

- (bool)checkFileValid:(NSString **)fileValidInfo;
- (NSMutableArray *)getVarsName;
- (NSMutableArray *)getVorDimsName:(NSString *)vorVarName;
- (bool)checkIfIsWrfoutFile:(NSString **)exceptionInfo;
- (int)getZLvDimLenName: (NSString **)zLvDimName;

@end

#endif /* NCFileInfo_Wrapper_h */
