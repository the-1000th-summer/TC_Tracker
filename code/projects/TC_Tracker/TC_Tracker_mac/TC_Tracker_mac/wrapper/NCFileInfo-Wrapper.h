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
- (void)dealloc;

- (bool)checkFileValid:(NSString **)fileValidInfo;
- (NSMutableArray *)getVarsName;

@end

#endif /* NCFileInfo_Wrapper_h */
