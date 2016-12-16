package com.mapbox.mapboxsdk.utils;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;

import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import static junit.framework.Assert.assertEquals;
import static org.junit.Assert.assertArrayEquals;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyByte;
import static org.mockito.Matchers.anyDouble;
import static org.mockito.Matchers.anyFloat;
import static org.mockito.Matchers.anyInt;
import static org.mockito.Matchers.anyLong;
import static org.mockito.Matchers.anyString;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MockParcel {

  public static Parcelable obtain(@NonNull Parcelable object) {
    return obtain(object, 0);
  }

  public static Parcelable obtain(@NonNull Parcelable object, int describeContentsValue) {
    testDescribeContents(object, describeContentsValue);
    testParcelableArray(object);
    return testParcelable(object);
  }

  public static Parcelable testParcelable(@NonNull Parcelable object) {
    Parcel parcel = ParcelMocker.obtain(object);
    object.writeToParcel(parcel, 0);
    parcel.setDataPosition(0);

    try {
      Field field = object.getClass().getDeclaredField("CREATOR");
      field.setAccessible(true);
      Class<?> creatorClass = field.getType();
      Object fieldValue = field.get(object);
      Method myMethod = creatorClass.getDeclaredMethod("createFromParcel", Parcel.class);
      return (Parcelable) myMethod.invoke(fieldValue, parcel);
    } catch (Exception exception) {
      return null;
    }
  }

  public static void testParcelableArray(@NonNull Parcelable object) {
    Parcelable[] objects = new Parcelable[] {object};
    Parcel parcel = ParcelMocker.obtain(objects);
    parcel.writeParcelableArray(objects, 0);
    parcel.setDataPosition(0);
    Parcelable[] parcelableArray = parcel.readParcelableArray(object.getClass().getClassLoader());
    assertArrayEquals("parcel should match initial object", objects, parcelableArray);
  }

  public static void testDescribeContents(@NonNull Parcelable object, int describeContentsValue) {
    if (describeContentsValue == 0) {
      assertEquals("\nExpecting a describeContents() value of 0 for a " + object.getClass().getSimpleName()
        + " instance." + "\nYou can provide a different value for describeContentValue through the obtain method.",
        0,
        object.describeContents());
    } else {
      assertEquals("Expecting a describeContents() value of " + describeContentsValue,
        describeContentsValue,
        object.describeContents());
    }
  }

  private static class ParcelMocker {

    public static Parcel obtain(@NonNull Parcelable target) {
      Parcel parcel = new ParcelMocker(target).getMockedParcel();
      target.writeToParcel(parcel, 0);
      parcel.setDataPosition(0);
      return parcel;
    }

    public static Parcel obtain(@NonNull Parcelable[] targets) {
      if (targets.length == 0) {
        throw new IllegalArgumentException("The passed argument may not be empty");
      }
      Parcel parcel = new ParcelMocker(targets[0]).getMockedParcel();
      parcel.writeParcelableArray(targets, 0);
      parcel.setDataPosition(0);
      return parcel;
    }

    private List<Object> objects;
    private Object object;
    private Parcel mockedParcel;
    private int position;

    private ParcelMocker(Object o) {
      this.object = o;
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
      Answer<Void> writeIntArrayAnswer = new Answer<Void>() {
        @Override
        public Void answer(InvocationOnMock invocation) throws Throwable {
          int[] parameters = (int[]) invocation.getArguments()[0];
          if (parameters != null) {
            objects.add(parameters.length);
            for (Object o : parameters) {
              objects.add(o);
            }
          } else {
            objects.add(-1);
          }
          return null;
        }
      };
      doAnswer(writeValueAnswer).when(mockedParcel).writeByte(anyByte());
      doAnswer(writeValueAnswer).when(mockedParcel).writeLong(anyLong());
      doAnswer(writeValueAnswer).when(mockedParcel).writeString(anyString());
      doAnswer(writeValueAnswer).when(mockedParcel).writeInt(anyInt());
      doAnswer(writeIntArrayAnswer).when(mockedParcel).writeIntArray(any(int[].class));
      doAnswer(writeValueAnswer).when(mockedParcel).writeDouble(anyDouble());
      doAnswer(writeValueAnswer).when(mockedParcel).writeFloat(anyFloat());
      doAnswer(writeValueAnswer).when(mockedParcel).writeParcelable(any(Parcelable.class), eq(0));
      doAnswer(writeArrayAnswer).when(mockedParcel).writeParcelableArray(any(Parcelable[].class), eq(0));
    }

    private void setupReads() {
      when(mockedParcel.readInt()).then(new Answer<Integer>() {
        @Override
        public Integer answer(InvocationOnMock invocation) throws Throwable {
          return (Integer) objects.get(position++);
        }
      });
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
      when(mockedParcel.readFloat()).thenAnswer(new Answer<Float>() {
        @Override
        public Float answer(InvocationOnMock invocation) throws Throwable {
          return (Float) objects.get(position++);
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
          Field field = object.getClass().getDeclaredField("CREATOR");
          field.setAccessible(true);
          Class<?> creatorClass = field.getType();
          Object fieldValue = field.get(object);
          Method myMethod = creatorClass.getDeclaredMethod("newArray", int.class);
          Object[] array = (Object[]) myMethod.invoke(fieldValue, size);
          for (int i = 0; i < size; i++) {
            array[i] = objects.get(position++);
          }
          return array;
        }
      });
      when(mockedParcel.createIntArray()).then(new Answer<int[]>() {
        @Override
        public int[] answer(InvocationOnMock invocation) throws Throwable {
          int size = (Integer) objects.get(position++);
          if (size == -1) {
            return null;
          }

          int[] array = new int[size];
          for (int i = 0; i < size; i++) {
            array[i] = (Integer) objects.get(position++);
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
}
