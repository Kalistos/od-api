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
}
