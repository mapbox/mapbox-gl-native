package com.mapbox.mapboxsdk.lint

import com.android.tools.lint.client.api.UElementHandler
import com.android.tools.lint.detector.api.*
import com.intellij.lang.jvm.JvmModifier
import com.intellij.psi.PsiType
import org.jetbrains.uast.*

class KeepDetector : Detector(), SourceCodeScanner, FileScanner {
  companion object {
    private val DETECTOR_CLASS = KeepDetector::class.java
    private val DETECTOR_SCOPE = Scope.JAVA_FILE_SCOPE
    private val IMPLEMENTATION = Implementation(DETECTOR_CLASS, DETECTOR_SCOPE)

    private const val ISSUE_ID = "KeepMissing"
    private const val ISSUE_DESCRIPTION = "Element cannot be minified."
    private const val ISSUE_EXPLANATION = "This class, method or field might contain native references. " +
      "It has to be annotated with @Keep if it cannot be obfuscated/removed, otherwise, the warning should be suppressed. " +
      "There can also be another methods/fields of this class that cannot be obfuscated/removed, " +
      "look closely for any methods that are referenced from the JNI context."
    private val ISSUE_CATEGORY = Category.CORRECTNESS
    private const val ISSUE_PRIORITY = 9
    private val ISSUE_SEVERITY = Severity.ERROR

    var ISSUE_NOT_KEPT = Issue.create(
      ISSUE_ID,
      ISSUE_DESCRIPTION,
      ISSUE_EXPLANATION,
      ISSUE_CATEGORY,
      ISSUE_PRIORITY,
      ISSUE_SEVERITY,
      IMPLEMENTATION
    )
  }

  override fun getApplicableUastTypes(): List<Class<out UElement>>? =
    listOf(UMethod::class.java, UField::class.java)

  override fun createUastHandler(context: JavaContext): UElementHandler? = KeepHandler(context)

  class KeepHandler(private val context: JavaContext) : UElementHandler() {

    override fun visitMethod(node: UMethod) {
      if (node.hasModifier(JvmModifier.NATIVE) && checkKeepAnnotation(node)) {
        context.report(ISSUE_NOT_KEPT, node,
          context.getNameLocation(node),
          "This method contains native references and will be minified.")
      } else if (node.isConstructor
        && node.parameterList.parameters.find { it.type == PsiType.LONG && it.name!!.contains("native") } != null
        && checkKeepAnnotation(node)) {
        context.report(ISSUE_NOT_KEPT, node,
          context.getNameLocation(node as UElement),
          "This constructor might contain native references and will be minified. " +
            "Either suppress the warning or use @Keep.")
      }
    }

    override fun visitField(node: UField) {
      if (node.type == PsiType.LONG && node.name.contains("native") && checkKeepAnnotation(node)) {
        context.report(ISSUE_NOT_KEPT, node,
          context.getNameLocation(node as UElement),
          "This field might contain native references and will be minified. " +
            "Either suppress the warning or use @Keep.")
      }
    }

    private fun checkKeepAnnotation(uAnnotated: UAnnotated): Boolean {
      return !hasKeepAnnotation(uAnnotated) && !hasKeepAnnotation(uAnnotated.getContainingUClass()!!)
    }

    private fun hasKeepAnnotation(uAnnotated: UAnnotated): Boolean {
      return uAnnotated.findAnnotation("android.support.annotation.Keep") != null
    }
  }
}