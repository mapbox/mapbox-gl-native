'use strict';

const template = {
  AWSTemplateFormatVersion: '2010-09-09',
  Description: 'mapbox-gl-native travis resources',
  Resources: {
    BuildUser: {
      Type: 'AWS::IAM::User',
      Properties: {
        Policies: [
          {
            PolicyName: 'listBuckets',
            PolicyDocument: {
              Statement: [
                {
                  Action: [ 's3:ListBucket' ],
                  Effect: 'Allow',
                  Resource: [ 'arn:aws:s3:::mapbox' ],
                  Condition: { StringLike: { 's3:prefix': 'mapbox-gl-native/*' } }
                },
                {
                  Action: [ 's3:ListBucket' ],
                  Resource: [ 'arn:aws:s3:::mapbox-node-binary' ],
                  Effect: 'Allow'
                },
                {
                  Action: [ 's3:ListBucket' ],
                  Resource: [ 'arn:aws:s3:::mapbox-loading-dock' ],
                  Effect: 'Allow'
                }
              ]
            }
          },
          {
            PolicyName: 'build-testing',
            PolicyDocument: {
              Statement: [
                {
                  Action: [
                    's3:GetObject',
                    's3:GetObjectAcl',
                    's3:PutObject',
                    's3:PutObjectAcl'
                  ],
                  Effect: 'Allow',
                  Resource: [
                    'arn:aws:s3:::mapbox/mapbox-gl-native/*',
                    'arn:aws:s3:::mapbox-node-binary/@mapbox/mapbox-gl-native/*',
                    'arn:aws:s3:::mapbox/mapbox-gl-native/ios/builds/*'
                  ]
                }
              ]
            }
          },
          {
            PolicyName: 'cloudwatch-metrics',
            PolicyDocument: {
              Statement: [
                {
                  Action: [
                    'cloudwatch:PutMetricData',
                    'cloudwatch:GetMetricData',
                    'cloudwatch:GetMetricStatistics'
                  ],
                  Effect: 'Allow',
                  Resource: [ '*' ]
                }
              ]
            }
          },
          {
            PolicyName: 'get-signing-key',
            PolicyDocument: {
              Statement: [
                {
                  Action: [ 's3:GetObject' ],
                  Effect: 'Allow',
                  Resource: [
                    'arn:aws:s3:::mapbox/android/signing-credentials/secring.gpg'
                  ]
                }
              ]
            }
          },
          {
            PolicyName: 'publish-metrics',
            PolicyDocument: {
              Statement: [
                {
                  Action: [ 's3:PutObject', 's3:GetObject', 's3:GetObjectAcl' ],
                  Effect: 'Allow',
                  Resource: [
                    'arn:aws:s3:::mapbox-loading-dock/raw/mobile.binarysize/*',
                    'arn:aws:s3:::mapbox-loading-dock/raw/mobile.codecoverage/*',
                    'arn:aws:s3:::mapbox-loading-dock/raw/mobile_staging.docs_coverage/*',
                    'arn:aws:s3:::mapbox-loading-dock/raw/mobile_staging.codecoverage/*',
                    'arn:aws:s3:::mapbox-loading-dock/raw/mobile_staging.github_stats/*'
                  ]
                }
              ]
            }
          }
        ]
      }
    },
    BuildUserKey: {
      Type: 'AWS::IAM::AccessKey',
      Properties: { UserName: { Ref: 'BuildUser' } }
    }
  },
  Outputs: {
    AccessKeyId: { Value: { Ref: 'BuildUserKey' } },
    SecretAccessKey: { Value: { 'Fn::GetAtt': [ 'BuildUserKey', 'SecretAccessKey' ] } }
  }
};

module.exports = template;
