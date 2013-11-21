<?php

/**
 * @file
 * Contains the OdApiPublisher class.
 */

abstract class OdApiPublisherDefault extends OdApiObject {
  public static $title = 'Ответственное лицо открытых данных';
  public static $type = self::OBJECT_TYPE_IS_PUBLISHER;
  public static $typeCardinality = self::OBJECT_CARDINALITY_IS_SINGLE;
  public static $parentType = self::OBJECT_TYPE_IS_META;
  public static $childrenTypes = array();

  // Errors.
  const SITE_OPENDATA_PUBLISHER_ARG_IS_VALID = 3101;

  public function set_message_text() {
    $this::$message_texts = array(
      $this::SITE_OPENDATA_PUBLISHER_ARG_IS_VALID => array(
        TRUE => 'Publisher arg is valid.',
        FALSE => 'Publisher arg is invalid.',
      ),
    );
  }
}
