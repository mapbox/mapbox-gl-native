package com.mapbox.mapboxsdk

import android.content.Context
import io.mockk.every
import io.mockk.mockk
import io.mockk.verify
import junit.framework.Assert.assertNotNull
import junit.framework.Assert.assertNull
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import java.io.File

@RunWith(RobolectricTestRunner::class)
class ModuleProviderTest {

    private val context: Context = mockk()
    private lateinit var moduleProvider: ModuleProvider

    @Before
    fun setUp() {
        moduleProvider = mockk()
        every {  moduleProvider.obtainTelemetry() } answers { mockk() }
        every { context.applicationContext } answers { context }
        every { context.filesDir } answers { File("") }
        every { context.cacheDir } answers { File("") }
    }

    @Test
    fun verifyTelemetryStartup() {
        Mapbox.setModuleProvider(moduleProvider)
        Mapbox.initialize(context, "pk.abcdefghijk")
        verify { moduleProvider.obtainTelemetry() }
        assertNotNull(Mapbox.getTelemetry())
    }

    @Test
    fun verifyTelemetryNotStartup() {
        Mapbox.setModuleProvider(moduleProvider)
        Mapbox.initialize(context, null)
        verify(exactly = 0) { moduleProvider.obtainTelemetry() }
        assertNull(Mapbox.getTelemetry())
    }

    @Test
    fun verifyLibraryLoader() {
        Mapbox.setModuleProvider(moduleProvider)
        Mapbox.initialize(context, null)
        verify { moduleProvider.obtainLibraryLoader() }
    }
}