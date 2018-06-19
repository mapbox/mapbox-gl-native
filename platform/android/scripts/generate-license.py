#!/usr/bin/python

import os
import json

path = os.getcwd() + "/platform/android/"
with open(path + "LICENSE.md", 'w') as licenseFile:
    licenseFile.write("<!-- This file was generated. Use `make android-license` to update. -->  \n")
    licenseFile.write("## Additional Mapbox GL licenses  \n")
    with open(path + "MapboxGLAndroidSDK/build/reports/licenses/licenseReleaseReport.json", 'r') as dataFile:
        data = json.load(dataFile)

        gradleLicensePlugin ="""
        {
            "project": "Gradle License Plugin",
            "url": "https://github.com/jaredsburrows/gradle-license-plugin",
            "licenses": [
                {
                    "license": "The Apache Software License, Version 2.0",
                    "license_url": "http://www.apache.org/licenses/LICENSE-2.0.txt"
                }
             ]
        }
        """
        data.append(json.loads(gradleLicensePlugin))

        licenseName = ""
        licenseUrl = ""
        for entry in data:
            projectName = entry["project"]
            projectUrl = entry["url"]
            for license in entry["licenses"]:
                licenseName = license["license"]
                licenseUrl = license["license_url"]

            licenseFile.write("Mapbox GL uses portions of the %s.  \n" % projectName +
                              ("URL: [%s](%s)  \n" % (projectUrl, projectUrl) if projectUrl is not None else "") +
                              "License: [%s](%s)" % (licenseName, licenseUrl) +
                              "\n\n===========================================================================\n\n")
