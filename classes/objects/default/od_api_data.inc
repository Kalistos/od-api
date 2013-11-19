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
}
