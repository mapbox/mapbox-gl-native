### Release `ios8-release-gazpacho`

This branch is intended to exist as a branch off of of the main Maps SDK. There are no plans to bring this branch back into `master`. If this branch is updated to incorporate new release branches in the future, please update the branch name.

Before running this script, please check that you are authorized to upload to S3 in China. 

1. Update the `IOS_SDK_VERSION` and `SEM_VERSION` values in the `platform/ios/scripts/package.sh` file. Versioning will follow: `vX.X.X-cn.X` in order to keep this distinct from the main Maps SDK.

2. Update the SDK version in the podspec files.

3. Update the `RELEASE_VERSION` and `SHORT_VERSION` in `platform/ios/scripts/documentation.sh`.

4. Run `platform/ios/scripts/publish-cn.sh`. This:
  - Builds `Mapbox.framework` and a companion dSYM file.
  - Generates documentation.
  - Uploads a .zip file containing the framework and documentation to S3 China.