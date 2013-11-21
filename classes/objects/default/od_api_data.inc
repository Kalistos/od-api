<?php

/**
 * @file
 * Contains the OdApiData class.
 */

abstract class OdApiDataDefault extends OdApiObject {
  public static $title = 'Набор открытых данных';
  public static $type = self::OBJECT_TYPE_IS_DATA;
  public static $typeCardinality = self::OBJECT_CARDINALITY_IS_MULTIPLE;
  public static $parentType = self::OBJECT_TYPE_IS_SOURCE;
  public static $childrenTypes = array(self::OBJECT_TYPE_IS_STRUCTURE);

  // Errors.
  const SITE_OPENDATA_DATA_EXISTS = 5101;

  public function set_message_text() {
    $this::$message_texts = array(
      $this::SITE_OPENDATA_DATA_EXISTS => array(
        TRUE => 'Data exists.',
        FALSE => 'Data does not exist.',
      ),
    );
  }
}
