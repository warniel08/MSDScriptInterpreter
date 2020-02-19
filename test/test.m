//
//  test.m
//  test
//
//  Created by Warner Nielsen on 1/22/20.
//  Copyright © 2020 Warner Nielsen. All rights reserved.
//

#import <XCTest/XCTest.h>

@interface test : XCTestCase
@end

@implementation test
- (void)testAll {
  if (!run_tests())
    XCTFail(@"failed");
}

@end
