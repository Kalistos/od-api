<?php

/**
 * @file
 * Contains the OdApiStructure class.
 */

abstract class OdApiStructureDefault extends OdApiObject {
  public static $title = 'Структура набора открытых данных';
  public static $type = self::OBJECT_TYPE_IS_STRUCTURE;
  public static $typeCardinality = self::OBJECT_CARDINALITY_IS_SINGLE;
  public static $parentType = self::OBJECT_TYPE_IS_DATA;
  public static $childrenTypes = array();

  // Errors.
  const SITE_OPENDATA_STRUCTURE_EXISTS = 6101;

  public function set_message_text() {
    $this::$message_texts = array(
      $this::SITE_OPENDATA_STRUCTURE_EXISTS => array(
        TRUE => 'Data exists.',
        FALSE => 'Data does not exist.',
      ),
    );
  }
}
