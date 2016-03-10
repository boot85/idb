// Copyright 2004-present Facebook. All Rights Reserved.

/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#import <Foundation/Foundation.h>

#import <FBControlCore/FBDebugDescribeable.h>
#import <FBControlCore/FBJSONConversion.h>

@class FBDiagnostic;

/**
 A Predicate for finding substrings in text.
 */
@interface FBLogSearchPredicate : NSObject <NSCopying, NSCoding, FBJSONSerializable, FBJSONDeserializable, FBDebugDescribeable>

/**
 A predicate that will match a line containing one of the substrings.
 Substrings cannot contain newline characters.

 @param substrings the substrings to search for.
 @return a Log Search Predicate.
 */
+ (instancetype)substrings:(NSArray *)substrings;

/**
 A predicate that will match a line matching the regular expression.

 @param regex a regex that will compile with NSRegularExpression
 @return a Log Search Predicate.
 */
+ (instancetype)regex:(NSString *)regex;

@end

/**
 Defines a model for batch searching diagnostics.
 This model is then used to concurrently search logs, returning the relevant matches.

 Diagnostics are defined in terms of thier short_name.
 Logs are defined in terms of Search Predicates.
 */
@interface FBBatchLogSearch : NSObject <NSCopying, NSCoding, FBJSONSerializable, FBJSONDeserializable, FBDebugDescribeable>

/**
 Constructs a Batch Log Search for the provided mapping of log names to predicates.
 The provided mapping is an NSDictionary where:
 - The keys are an NSArray of NSStrings of the names of the Logs to search. An Empty list means that the value will apply to all predicates.
 - The values are an NSArray of FBLogSearchPredicates of the predicates to search the named logs with.

 @param mapping the mapping to search with.
 @param error an error out for any error in the mapping format.
 @return an FBBatchLogSearch instance if the mapping is valid, nil otherwise.
 */
+ (instancetype)withMapping:(NSDictionary *)mapping error:(NSError **)error;

/**
 Runs the Reciever over an array of Diagnostics.
 The returned dictionary is a NSDictionary where:
 - The keys are the log names. A log must have 1 or more matches to have a key.
 - The values are an NSArrray of NSStrings for the lines that have been matched.

 @param diagnostics an NSArray of FBDiagnostics to search.
 @return an NSDictionary mapping log names to the matching lines that were found in the diagnostics.
 */
- (NSDictionary *)search:(NSArray *)diagnostics;

/**
 Convenience method for searching an array of diagnostics with a single predicate.

 @param diagnostics an NSArray of FBDiagnostics to search.
 @param predicate a Log Search Predicate to search with.
 @return an NSDictionary mapping log names to the matching lines that were found in the diagnostics.
 */
+ (NSDictionary *)searchDiagnostics:(NSArray *)diagnostics withPredicate:(FBLogSearchPredicate *)predicate;

@end

/**
 Wraps FBDiagnostic with Log Searching Abilities.
 */
@interface FBLogSearch : NSObject

/**
 Creates a Log Searcher for the given diagnostic.

 @param diagnostic the diagnostic to search.
 @param predicate the predicate to search with.
 */
+ (instancetype)withDiagnostic:(FBDiagnostic *)diagnostic predicate:(FBLogSearchPredicate *)predicate;

/**
 Searches the Diagnostic Log, returning the first match of the predicate.
 If the Diagnostic is not searchable as text, nil will be returned.

 Most Diagnostics have effectively constant content, except for file backed diagnostics.
 For this reason, the result returned from this method may change if the file backing the diagnostic changes.

 @return the first match of the predicate in the diagnostic, nil if nothing was found.
 */
- (NSString *)firstMatch;

/**
 Searches the Diagnostic Log, returning the line where the first match was found.
 If the Diagnostic is not searchable as text, nil will be returned.

 Most Diagnostics have effectively constant content, except for file backed diagnostics.
 For this reason, the result returned from this method may change if the file backing the diagnostic changes.

 @return the first line matching the predicate in the diagnostic, nil if nothing was found.
 */
- (NSString *)firstMatchingLine;

/**
 The Diagnostic to Search.
 */
@property (nonatomic, copy, readonly) FBDiagnostic *diagnostic;

/**
 The Predicate to Search with.
 */
@property (nonatomic, copy, readonly) FBLogSearchPredicate *predicate;

@end
