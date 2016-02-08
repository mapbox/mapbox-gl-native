package com.mapbox.mapboxsdk.utils;

import android.os.Parcel;

import com.mapbox.mapboxsdk.geometry.LatLng;

import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import java.util.ArrayList;
import java.util.List;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyDouble;
import static org.mockito.Matchers.anyInt;
import static org.mockito.Matchers.anyLong;
import static org.mockito.Matchers.anyString;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MockParcel {

    public static Parcel obtain() {
        return new MockParcel().getMockedParcel();
    }

    Parcel mockedParcel;
    int position;
    List<Object> objects;

    public Parcel getMockedParcel() {
        return mockedParcel;
    }

    public MockParcel() {
        mockedParcel = mock(Parcel.class);
        objects = new ArrayList<>();
        setupMock();
    }

    private void setupMock() {
        setupWrites();
        setupReads();
        setupOthers();
    }

    private void setupWrites() {
        Answer<Void> writeValueAnswer = new Answer<Void>() {
            @Override
            public Void answer(InvocationOnMock invocation) throws Throwable {
                Object parameter = invocation.getArguments()[0];
                objects.add(parameter);
                return null;
            }
        };
        Answer<Void> writeArrayAnswer = new Answer<Void>() {
            @Override
            public Void answer(InvocationOnMock invocation) throws Throwable {
                Object[] parameters = (Object[]) invocation.getArguments()[0];
                objects.add(parameters.length);
                for (Object o : parameters) {
                    objects.add(o);
                }
                return null;
            }
        };

        doAnswer(writeValueAnswer).when(mockedParcel).writeLong(anyLong());
        doAnswer(writeValueAnswer).when(mockedParcel).writeString(anyString());
        doAnswer(writeValueAnswer).when(mockedParcel).writeDouble(anyDouble());
        doAnswer(writeArrayAnswer).when(mockedParcel).writeParcelableArray(any(LatLng[].class), eq(0));
    }

    private void setupReads() {
        when(mockedParcel.readLong()).thenAnswer(new Answer<Long>() {
            @Override
            public Long answer(InvocationOnMock invocation) throws Throwable {
                return (Long) objects.get(position++);
            }
        });
        when(mockedParcel.readString()).thenAnswer(new Answer<String>() {
            @Override
            public String answer(InvocationOnMock invocation) throws Throwable {
                return (String) objects.get(position++);
            }
        });
        when(mockedParcel.readDouble()).thenAnswer(new Answer<Double>() {
            @Override
            public Double answer(InvocationOnMock invocation) throws Throwable {
                return (Double) objects.get(position++);
            }
        });
        when(mockedParcel.readParcelableArray(LatLng.class.getClassLoader())).thenAnswer(new Answer<LatLng[]>() {
            @Override
            public LatLng[] answer(InvocationOnMock invocation) throws Throwable {
                int size = (Integer) objects.get(position++);
                LatLng[] latLngs = LatLng.CREATOR.newArray(size);
                for (int i = 0; i < size; i++) {
                    latLngs[i] = (LatLng) objects.get(position++);
                }
                return latLngs;
            }
        });
    }

    private void setupOthers() {
        doAnswer(new Answer<Void>() {
            @Override
            public Void answer(InvocationOnMock invocation) throws Throwable {
                position = ((Integer) invocation.getArguments()[0]);
                return null;
            }
        }).when(mockedParcel).setDataPosition(anyInt());
    }

}
