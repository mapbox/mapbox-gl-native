#!/usr/bin/swift

/// Generates Entitlement files .xcent for codesigning, containing the correct team id
import Swift
import Foundation

let fileManager = FileManager.default

let contentsOfCurrentWorkingDirectory = try FileManager.default.contentsOfDirectory(at: URL(fileURLWithPath: "./codesigning/"), includingPropertiesForKeys: nil, options: [])

guard let rawTeamIdEnv = getenv("IOS_DEVELOPMENT_TEAM") else {
    print("Please add a IOS_DEVELOPMENT_TEAM environment variable.")
    exit(1)
}

let teamId2 = String(utf8String: rawTeamIdEnv)

for file in contentsOfCurrentWorkingDirectory {
    if (file.pathExtension == "template") {
        var content = ""
        do {
            content = try String(contentsOf: file, encoding: .utf8)
        }
        catch {
            print("Error reading xcent file: \(error).")
        }

        content = content.replacingOccurrences(of: "$TEAM_ID", with: teamId2!)

        var targetFile = file
        targetFile.deletePathExtension()
        do {
            try content.write(to: targetFile, atomically: false, encoding: .utf8)
            print("Entitlement " + targetFile.absoluteString + " generated.")
        }
        catch {
            print("Error writing xcent file: \(error).")
        }
    }
}