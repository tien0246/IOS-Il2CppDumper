#include <cstdio>
#include <string>
#include <fstream>
#include <utility>

#import <SCLAlertView/SCLAlertView.h>
#import <SSZipArchive/ZipArchive.h>

#include "AlertUtils.h"

#include "Core/Il2cpp.hpp"
#include "Core/Dumper.hpp"

#include "Core/config.h"

void dump_thread();

__attribute__((constructor)) static void onLoad()
{
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        NSLog(@"=================STRAT DUMPPER=================");
        dump_thread();
        NSLog(@"=================END DUMPPER=================");
    });
}

void dump_thread()
{
  sleep(5);

  showInfo([NSString stringWithFormat:@"Dumping after %d seconds.", WAIT_TIME_SEC], WAIT_TIME_SEC / 2);

  sleep(WAIT_TIME_SEC);

  NSString *docDir = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)[0];

  NSString *appName = [[[NSBundle mainBundle] infoDictionary] objectForKey:(id)kCFBundleNameKey];

  NSString *dumpFolderName = [NSString stringWithFormat:@"%@_%s", [appName stringByReplacingOccurrencesOfString:@" " withString:@""], DUMP_FOLDER];

  NSString *dumpPath = [NSString stringWithFormat:@"%@/%@", docDir, dumpFolderName];
  NSString *headersdumpPath = [NSString stringWithFormat:@"%@/%@", dumpPath, @"Assembly"];
  NSString *zipdumpPath = [NSString stringWithFormat:@"%@.zip", dumpPath];

  NSString *appPath = [[NSBundle mainBundle] bundlePath];
  NSString *binaryPath = [NSString stringWithFormat:@"%s", BINARY_NAME];
  if ([binaryPath isEqualToString:@"UnityFramework"])
  {
    binaryPath = [appPath stringByAppendingPathComponent:@"Frameworks/UnityFramework.framework/UnityFramework"];
  }
  else
  {
    binaryPath = [appPath stringByAppendingPathComponent:binaryPath];
  }

  Variables::IL2CPP::processAttach(binaryPath.UTF8String);

  NSLog(@"UNITY_PATH: %@", dumpPath);

  NSFileManager *fileManager = [NSFileManager defaultManager];

  if ([fileManager fileExistsAtPath:dumpPath])
  {
    [fileManager removeItemAtPath:dumpPath error:nil];
  }
  if ([fileManager fileExistsAtPath:zipdumpPath])
  {
    [fileManager removeItemAtPath:zipdumpPath error:nil];
  }

  NSError *error = nil;
  if (![fileManager createDirectoryAtPath:headersdumpPath withIntermediateDirectories:YES attributes:nil error:&error])
  {
    NSLog(@"Failed to create folders.\nError: %@", error);
    showError([NSString stringWithFormat:@"Failed to create folders.\nError: %@", error]);
    return;
  }

  SCLAlertView *waitingAlert = nil;
  showWaiting(@"Dumping...", &waitingAlert);
  
  Dumper::dump(dumpPath.UTF8String, headersdumpPath.UTF8String);

  if ([fileManager fileExistsAtPath:dumpPath])
  {
    [SSZipArchive createZipFileAtPath:zipdumpPath withContentsOfDirectory:dumpPath];
    [fileManager removeItemAtPath:dumpPath error:nil];
  }

  dismisWaiting(waitingAlert);

  if (Dumper::status != Dumper::DumpStatus::SUCCESS) {
    if (Dumper::status == Dumper::DumpStatus::ERROR) {
      showError(@"Error while dumping, check logs.txt");
    }
  }

  NSLog(@"Dump finished.");

  dispatch_async(dispatch_get_main_queue(), ^{
    
    SCLAlertView *okAlert = [[SCLAlertView alloc] initWithNewWindow];
    okAlert.shouldDismissOnTapOutside = YES;
    okAlert.showAnimationType = SCLAlertViewShowAnimationSlideInFromTop;
    okAlert.hideAnimationType = SCLAlertViewHideAnimationSlideOutToBottom;

    [okAlert showSuccess:@___ALERT_TITLE subTitle:[NSString stringWithFormat:@"Dump at: \n%@", zipdumpPath] closeButtonTitle:@"Ok" duration:0.0f];

  });
}
