package com.mapbox.mapboxsdk.utils;

import android.os.Parcel;
import android.os.Parcelable;

import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyByte;
import static org.mockito.Matchers.anyDouble;
import static org.mockito.Matchers.anyInt;
import static org.mockito.Matchers.anyLong;
import static org.mockito.Matchers.anyString;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MockParcel {

    public static Parcel obtain(Parcelable target) {
        Parcel parcel = new MockParcel(target).getMockedParcel();
        target.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        return parcel;
    }

    public static Parcel obtain(Parcelable[] targets) {
        if (targets == null || targets.length == 0) {
            throw new IllegalArgumentException("The passed argument may not be null or empty");
        }
        Parcel parcel = new MockParcel(targets[0]).getMockedParcel();
        parcel.writeParcelableArray(targets, 0);
        parcel.setDataPosition(0);
        return parcel;
    }

    private Parcel mockedParcel;
    private int position;
    private List<Object> objects;
    private Object o;

    private MockParcel(Object o) {
        this.o = o;
        mockedParcel = mock(Parcel.class);
        objects = new ArrayList<>();
        setupMock();
    }

    private Parcel getMockedParcel() {
        return mockedParcel;
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
        doAnswer(writeValueAnswer).when(mockedParcel).writeByte(anyByte());
        doAnswer(writeValueAnswer).when(mockedParcel).writeLong(anyLong());
        doAnswer(writeValueAnswer).when(mockedParcel).writeString(anyString());
        doAnswer(writeValueAnswer).when(mockedParcel).writeDouble(anyDouble());
        doAnswer(writeValueAnswer).when(mockedParcel).writeParcelable(any(Parcelable.class), eq(0));
        doAnswer(writeArrayAnswer).when(mockedParcel).writeParcelableArray(any(Parcelable[].class), eq(0));
    }

    private void setupReads() {
        when(mockedParcel.readByte()).thenAnswer(new Answer<Byte>() {
            @Override
            public Byte answer(InvocationOnMock invocation) throws Throwable {
                return (Byte) objects.get(position++);
            }
        });
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
        when(mockedParcel.readParcelable(Parcelable.class.getClassLoader())).thenAnswer(new Answer<Parcelable>() {
            @Override
            public Parcelable answer(InvocationOnMock invocation) throws Throwable {
                return (Parcelable) objects.get(position++);
            }
        });
        when(mockedParcel.readParcelableArray(Parcelable.class.getClassLoader())).thenAnswer(new Answer<Object[]>() {
            @Override
            public Object[] answer(InvocationOnMock invocation) throws Throwable {
                int size = (Integer) objects.get(position++);
                Field field = o.getClass().getDeclaredField("CREATOR");
                field.setAccessible(true);
                Class<?> creatorClass = field.getType();
                Object fieldValue = field.get(o);
                Method myMethod = creatorClass.getDeclaredMethod("newArray", int.class);
                Object[] array = (Object[]) myMethod.invoke(fieldValue, size);
                for (int i = 0; i < size; i++) {
                    array[i] = objects.get(position++);
                }
                return array;
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
