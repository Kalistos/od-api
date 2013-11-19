<?php

/**
 * @file
 * Contains the OdApi class.
 */

class OdApi {
  public $root;
  public $main_page_url;
  public $opendata_page_url;

  public $args;
  public $currentArgs;
  public $page;
  public $valid;
  public $result;
  public $version;

  public $objects;

  public function __construct() {
    $this->versions = array(2.3); // scandir(getcwd() . '/objects/versions');
    $this->root = OdApiObject::OBJECT_TYPE_IS_LIST;

    return $this;
  }

  public function execute($args) {
    // Get root type.
    $type = $this->root;

    // Init current args.
    $this->currentArgs = array();

    // Get current type arg.
    $arg = isset($args[$type]) ? $args[$type] : NULL;

    // Allocate current type objects (will be overwritten by cycle).
    $objects = $this->objects;

    // Walk through args to children objects.
    while ($arg !== NULL) {
      // Add current type to current args.
      $this->currentArgs[$type] = $args[$type];

      if (empty($objects[$type][$arg])) {
        foreach ($this->versions as $version) {
          $parent = !empty($object) ? $object : $this;
          $parent_object = !empty($object) ? $object : NULL;

          $class_name = $this->getClassNameByType($type, $version);

          $possible_object = new $class_name($this, $parent_object);

          // kpr(array($possible_object));
          if ($type === 'meta') {
            die;
          }
          $arg_is_valid = $possible_object->argIsValid;
          if (!$arg_is_valid) {
            continue;
          }

          $version_is_valid = $possible_object->versionIsValid;
          if (!$version_is_valid) {
            continue;
          }

          $object = clone $possible_object;

          $object_is_valid = $object->objectIsValid;

          $multiple = $object::$typeCardinality === OdApiObject::OBJECT_CARDINALITY_IS_MULTIPLE;

          if ($multiple) {
            if (!isset($this->objects[$type])) {
              $parent->objects[$type] = array();
            }

            $parent->objects[$type][$arg] = $object;
          }
          else {
            $parent->objects[$type] = $object;
          }

          if (!$object_is_valid) {
            break;
          }
        }
      }
      else {
        $object = $objects[$type][$arg];
      }

      if (empty($object)) {
        break;
      }

      $children_types = $object::$childrenTypes;

      $children = FALSE;
      foreach ($children_types as $children_type) {
        if (isset($args[$children_type])) {
          $children = TRUE;
          $type = $children_type;
          $arg = $args[$children_type];
          $objects = $object->objects;
        }
      }

      if (!$children) {
        $arg = NULL;
      }
    }
  }

  public function getResult($args = NULL) {
    // Get current args if incoming args is missing.
    if (empty($args)) {
      if (!empty($this->currentArgs)) {
        $args = $this->currentArgs;
      }
      else {
        return FALSE;
      }
    }

    $this->execute($args);

    $type = $this->root;
    $arg = isset($args[$type]) ? $args[$type] : NULL;
    $objects = $this->objects;

    while ($arg !== NULL) {
      $object = $objects[$type][$arg];
      $children_types = $object::$childrenTypes;

      $children = FALSE;
      foreach ($children_types as $children_type) {
        if (isset($args[$children_type])) {
          $children = TRUE;
          $arg = $args[$children_type];
          $objects = $object->objects;
        }
      }

      if (!$children) {
        $arg = NULL;
      }
    }

    return !empty($object) ? $object->getResult() : FALSE;
  }

  private function getClassNameByType($type, $version = NULL) { // @todo Сделать.
    $version = empty($version) ? 'Default' : filter_var($version, FILTER_SANITIZE_NUMBER_INT);
    return 'OdApi' . ucfirst($type) . $version;
  }
}
