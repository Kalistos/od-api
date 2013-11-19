<?php

/**
 * @file
 * Contains the OdApiSource class.
 */

abstract class OdApiSourceDefault extends OdApiObject {
  public static $title = 'Перечень версий наборов открытых данных';
  public static $type = self::OBJECT_TYPE_IS_SOURCE;
  public static $typeCardinality = self::OBJECT_CARDINALITY_IS_SINGLE;
  public static $parentType = self::OBJECT_TYPE_IS_META;
  public static $childrenTypes = array(self::OBJECT_TYPE_IS_DATA);
}
