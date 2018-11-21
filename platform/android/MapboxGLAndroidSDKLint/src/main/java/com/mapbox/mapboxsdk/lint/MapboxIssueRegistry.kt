package com.mapbox.mapboxsdk.lint

import com.android.tools.lint.client.api.IssueRegistry
import com.android.tools.lint.detector.api.Issue

class MapboxIssueRegistry : IssueRegistry() {
  override val issues: List<Issue>
    get() = listOf(KeepDetector.ISSUE_NOT_KEPT)
}