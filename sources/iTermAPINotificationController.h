//
//  iTermAPINotificationController.h
//  iTerm2SharedARC
//
//  Created by George Nachman on 02/05/19.
//

#import <Foundation/Foundation.h>

#import "Api.pbobjc.h"
#import "iTermTuple.h"

NS_ASSUME_NONNULL_BEGIN

@class iTermVariableScope;
@class PTYTab;
@class PseudoTerminal;
@class PTYSession;

extern NSString *const iTermAPIHelperFunctionCallErrorUserInfoKeyConnection;
extern NSString *const iTermAPIRegisteredFunctionsDidChangeNotification;
extern NSString *const iTermAPIDidRegisterSessionTitleFunctionNotification;
extern NSString *const iTermAPIDidRegisterStatusBarComponentNotification;  // object is the unique id of the status bar component
extern NSString *const iTermRemoveAPIServerSubscriptionsNotification;

extern const NSInteger iTermAPIHelperFunctionCallUnregisteredErrorCode;
extern const NSInteger iTermAPIHelperFunctionCallOtherErrorCode;

typedef void (^iTermServerOriginatedRPCCompletionBlock)(id _Nullable, NSError * _Nullable);

@interface iTermSessionTitleProvider : NSObject
@property (nonatomic, readonly) NSString *displayName;
@property (nonatomic, readonly) NSString *invocation;
@property (nonatomic, readonly) NSString *uniqueIdentifier;
@end

@protocol iTermAPINotificationControllerDelegate<NSObject>

- (void)apiNotificationControllerPostNotification:(ITMNotification *)notification
                                    connectionKey:(NSString *)key;

- (PTYTab *)apiNotificationControllerTabWithID:(NSString *)tabID;

- (PseudoTerminal *)apiNotificationControllerWindowControllerWithID:(NSString *)windowID;

- (PTYSession *)apiNotificationControllerSessionForAPIIdentifier:(NSString *)identifier
                                           includeBuriedSessions:(BOOL)includeBuriedSessions;

- (ITMListSessionsResponse *)apiNotificationControllerListSessionsResponse;

- (void)apiNotificationControllerLogToConnectionWithKey:(NSString *)connectionKey
                                                 string:(NSString *)string;

- (nullable NSString *)apiNotificationControllerFullPathOfScriptWithConnectionKey:(NSString *)connectionKey;

- (NSArray<PTYSession *> *)apiNotificationControllerAllSessions;

- (void)apiNotificationControllerEnumerateBroadcastDomains:(void (^)(NSArray<PTYSession *> *))addDomain;

@end

@interface iTermAPINotificationController : NSObject

@property (nonatomic, weak) id<iTermAPINotificationControllerDelegate> delegate;
@property (nonatomic, readonly) NSDictionary<NSString *, iTermTuple<id, ITMNotificationRequest *> *> *serverOriginatedRPCSubscriptions;
@property (nonatomic, readonly) NSDictionary<NSString *, NSArray<NSString *> *> *registeredFunctionSignatureDictionary;
@property (nonatomic, readonly) NSArray<ITMRPCRegistrationRequest *> *statusBarComponentProviderRegistrationRequests;
@property (nonatomic, readonly) NSArray<iTermSessionTitleProvider *> *sessionTitleFunctions;

+ (NSString *)invocationWithName:(NSString *)name
                        defaults:(NSArray<ITMRPCRegistrationRequest_RPCArgument*> *)defaultsArray;
+ (NSString *)nameOfScriptVendingStatusBarComponentWithUniqueIdentifier:(NSString *)uniqueID;

- (void)dispatchRPCWithName:(NSString *)name
                  arguments:(NSDictionary *)arguments
                 completion:(iTermServerOriginatedRPCCompletionBlock)completion;

// stringSignature is like func(arg1,arg2). Use iTermFunctionSignatureFromNameAndArguments to construct it safely.
- (BOOL)haveRegisteredFunctionWithSignature:(NSString *)stringSignature;

- (void)logToConnectionHostingFunctionWithSignature:(nullable NSString *)signatureString
                                             format:(NSString *)format, ...;

- (void)logToConnectionHostingFunctionWithSignature:(nullable NSString *)signatureString
                                             string:(NSString *)string;

- (NSString *)connectionKeyForRPCWithName:(NSString *)name
                       explicitParameters:(NSDictionary<NSString *, id> *)explicitParameters
                                    scope:(iTermVariableScope *)scope
                           fullParameters:(out NSDictionary<NSString *, id> **)fullParameters;

- (NSString *)connectionKeyForRPCWithSignature:(NSString *)signature;

#pragma mark - Internal

- (void)apiServerNotification:(ITMNotificationRequest *)request
                connectionKey:(NSString *)connectionKey
                      handler:(void (^)(ITMNotificationResponse *))handler;

- (void)didCloseConnectionWithKey:(id)connectionKey;

- (void)serverOriginatedRPCDidReceiveResponseWithResult:(ITMServerOriginatedRPCResultRequest *)result
                                          connectionKey:(NSString *)connectionKey;

- (void)stop;


@end

NS_ASSUME_NONNULL_END
