//
//  AINameStore.h
//  CloudStoreMobile
//
//  Created by Hans Malherbe on 2018/08/26.
//  Copyright © 2018 AiNET. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol AINameStore <NSObject>
@property (nonatomic) NSMutableArray<NSString*> *fileNames;
@property (nonatomic) NSMutableArray<NSString*> *directoryNames;
@end
