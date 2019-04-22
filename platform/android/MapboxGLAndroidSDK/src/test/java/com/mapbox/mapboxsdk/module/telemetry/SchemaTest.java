package com.mapbox.mapboxsdk.module.telemetry;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.JsonPrimitive;
import com.google.gson.annotations.SerializedName;

import org.apache.commons.io.IOUtils;
import org.junit.BeforeClass;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.GZIPInputStream;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class SchemaTest {
  private static final String MAP_CLICK = "map.click";
  private static final String MAP_DRAG = "map.dragend";
  private static final String MAP_LOAD = "map.load";
  private static ArrayList<JsonObject> schemaArray;

  @BeforeClass
  public static void downloadSchema() throws Exception {
    unpackSchemas();
  }

  @Test
  public void checkMapClickEventSize() throws Exception {
    JsonObject schema = grabSchema(MAP_CLICK);
    List<Field> fields = grabClassFields(MapClickEvent.class);

    assertEquals(schema.size(), fields.size());
  }

  @Test
  public void checkMapClickEventFields() throws Exception {
    JsonObject schema = grabSchema(MAP_CLICK);
    List<Field> fields = grabClassFields(MapClickEvent.class);

    schemaContainsFields(schema, fields);
  }

  @Test
  public void checkMapDragEndEventSize() throws Exception {
    JsonObject schema = grabSchema(MAP_DRAG);
    List<Field> fields = grabClassFields(MapDragendEvent.class);

    assertEquals(schema.size(), fields.size());
  }

  @Test
  public void checkMapDragEndEventFields() throws Exception {
    JsonObject schema = grabSchema(MAP_DRAG);
    List<Field> fields = grabClassFields(MapDragendEvent.class);

    schemaContainsFields(schema, fields);
  }

  @Test
  public void checkMapLoadEventSize() throws Exception {
    JsonObject schema = grabSchema(MAP_LOAD);
    List<Field> fields = grabClassFields(MapLoadEvent.class);

    //FIXME: this assertion is invalid: we should introduce a concept of mandatory/optional field to schema validation
    //assertEquals(schema.size(), fields.size());
  }

  @Test
  public void checkMapLoadEventFields() throws Exception {
    JsonObject schema = grabSchema(MAP_LOAD);
    List<Field> fields = grabClassFields(MapLoadEvent.class);

    schemaContainsFields(schema, fields);
  }

  private void schemaContainsFields(JsonObject schema, List<Field> fields) {
    int distanceRemainingCount = 0;
    int durationRemainingCount = 0;

    for (int i = 0; i < fields.size(); i++) {
      String thisField = String.valueOf(fields.get(i));
      String[] fieldArray = thisField.split(" ");
      String[] typeArray = fieldArray[fieldArray.length - 2].split("\\.");
      String type = typeArray[typeArray.length - 1];

      String[] nameArray = fieldArray[fieldArray.length - 1].split("\\.");
      String field = nameArray[nameArray.length - 1];

      SerializedName serializedName = fields.get(i).getAnnotation(SerializedName.class);

      if (serializedName != null) {
        field = serializedName.value();
      }

      if (field.equalsIgnoreCase("durationRemaining")) {
        durationRemainingCount++;

        if (durationRemainingCount > 1) {
          field = "step" + field;
        }
      }

      if (field.equalsIgnoreCase("distanceRemaining")) {
        distanceRemainingCount++;

        if (distanceRemainingCount > 1) {
          field = "step" + field;
        }
      }

      JsonObject thisSchema = findSchema(schema, field);
      assertNotNull(thisSchema);

      if (thisSchema.has("type")) {
        typesMatch(thisSchema, type);
      }
    }
  }

  private JsonObject findSchema(JsonObject schema, String field) {
    JsonObject thisSchema = schema.getAsJsonObject(field);

    return thisSchema;
  }

  private void typesMatch(JsonObject schema, String type) {
    if (type.equalsIgnoreCase("int") || type.equalsIgnoreCase("integer")
      || type.equalsIgnoreCase("double") || type.equalsIgnoreCase("float")) {
      type = "number";
    }

    if (type.contains("[]")) {
      type = "array";
    }

    Class<? extends JsonElement> typeClass = schema.get("type").getClass();
    JsonElement jsonElement = new JsonParser().parse(type.toLowerCase());

    if (typeClass == JsonPrimitive.class) {
      JsonElement typePrimitive = schema.get("type");
      assertTrue(typePrimitive.equals(jsonElement));
    } else {
      JsonArray arrayOfTypes = schema.getAsJsonArray("type");
      assertTrue(arrayOfTypes.contains(jsonElement));
    }
  }

  private static ByteArrayInputStream getFileBytes() throws IOException {
    InputStream inputStream = SchemaTest.class.getClassLoader().getResourceAsStream("mobile-event-schemas.jsonl.gz");
    byte[] byteOut = IOUtils.toByteArray(inputStream);

    return new ByteArrayInputStream(byteOut);
  }

  private static void unpackSchemas() throws IOException {
    ByteArrayInputStream bais = getFileBytes();
    GZIPInputStream gzis = new GZIPInputStream(bais);
    InputStreamReader reader = new InputStreamReader(gzis);
    BufferedReader in = new BufferedReader(reader);

    schemaArray = new ArrayList<>();

    Gson gson = new Gson();
    String readed;
    while ((readed = in.readLine()) != null) {
      JsonObject schema = gson.fromJson(readed, JsonObject.class);
      schemaArray.add(schema);
    }
  }

  private JsonObject grabSchema(String eventName) {
    for (JsonObject thisSchema : schemaArray) {
      String name = thisSchema.get("name").getAsString();

      if (name.equalsIgnoreCase(eventName)) {
        Gson gson = new Gson();
        String schemaString = gson.toJson(thisSchema.get("properties"));
        JsonObject schema = gson.fromJson(thisSchema.get("properties"), JsonObject.class);

        if (schema.has("step")) {
          JsonObject stepJson = schema.get("step").getAsJsonObject();
          JsonObject stepProperties = stepJson.get("properties").getAsJsonObject();

          String stepPropertiesJson = gson.toJson(stepProperties);
          schemaString = generateStepSchemaString(stepPropertiesJson, schemaString);

          schema = gson.fromJson(schemaString, JsonObject.class);
          schema.remove("step");
        }

        schema.remove("userAgent");
        schema.remove("received");
        schema.remove("token");
        schema.remove("authorization");
        schema.remove("owner");
        schema.remove("locationAuthorization");
        schema.remove("locationEnabled");
        //temporary need to work out a solution to include this data
        schema.remove("platform");

        return schema;
      }
    }

    return null;
  }

  private List<Field> grabClassFields(Class aClass) {
    List<Field> fields = new ArrayList<>();
    Field[] allFields = aClass.getDeclaredFields();
    for (Field field : allFields) {
      if (Modifier.isPrivate(field.getModifiers()) && !Modifier.isStatic(field.getModifiers())) {
        fields.add(field);
      }
    }
    Field[] superFields = aClass.getSuperclass().getDeclaredFields();
    for (Field field : superFields) {
      if (Modifier.isPrivate(field.getModifiers()) && !Modifier.isStatic(field.getModifiers())) {
        fields.add(field);
      }
    }
    return fields;
  }

  private List<Field> removeField(List<Field> fields, String fieldName) {
    for (Field field : new ArrayList<>(fields)) {
      String thisField = String.valueOf(field);
      String[] fieldArray = thisField.split("\\.");
      String simpleField = fieldArray[fieldArray.length - 1];
      if (simpleField.equalsIgnoreCase(fieldName)) {
        fields.remove(field);
      }
    }

    return fields;
  }

  private String generateStepSchemaString(String stepJson, String schemaString) {
    stepJson = stepJson.replace("\"distanceRemaining\"", "\"stepdistanceRemaining\"");
    stepJson = stepJson.replace("durationRemaining", "stepdurationRemaining");
    stepJson = stepJson.replaceFirst("\\{", ",");
    schemaString = schemaString.replaceAll("}$", "");
    schemaString = schemaString + stepJson;

    return schemaString;
  }
}
