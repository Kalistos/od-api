<?php

/**
 * @file
 * Contains the OdApiList class.
 */

abstract class OdApiListDefault extends OdApiObject {
  public static $title = 'Реестр открытых данных';
  public static $type = self::OBJECT_TYPE_IS_LIST;
  public static $typeCardinality = self::OBJECT_CARDINALITY_IS_MULTIPLE;
  public static $parentType = FALSE;
  public static $childrenTypes = array(self::OBJECT_TYPE_IS_META);

  // Errors.
  const SITE_URL_IS_VALID = 1101;
  const SITE_MAIN_PAGE_EXISTS = 1102;
  const SITE_OPENDATA_PAGE_EXISTS = 1103;

  public function set_message_text() {
    $this::$message_texts = array(
      $this::SITE_URL_IS_VALID => array(
        TRUE => 'Site url is valid.',
        FALSE => 'Site url is invalid.',
      ),
      $this::SITE_MAIN_PAGE_EXISTS => array(
        TRUE => 'Site main page exists.',
        FALSE => 'Site main page does not exist.',
      ),
      $this::SITE_OPENDATA_PAGE_EXISTS => array(
        TRUE => 'Site opendata page exists.',
        FALSE => 'Site opendata page does not exist.',
      ),
    );
  }
}
