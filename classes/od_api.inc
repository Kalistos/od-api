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

      // Get current object, which nesting depends on cardinality.
      $class_name = $this->getClassNameByType($type);
      $multiple_cardinality = $class_name::$typeCardinality === OdApiObject::OBJECT_CARDINALITY_IS_MULTIPLE;
      $object_exists = $multiple_cardinality ? !empty($objects[$type][$arg]) : !empty($objects[$type]);

      if (!$object_exists) {
        foreach ($this->versions as $version) {
          $parent = !empty($object) ? $object : $this;
          $parent_object = !empty($object) ? $object : NULL;

          $class_name = $this->getClassNameByType($type, $version);

          $possible_object = new $class_name($this, $parent_object);

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

          $multiple_cardinality = $class_name::$typeCardinality === OdApiObject::OBJECT_CARDINALITY_IS_MULTIPLE;

          if ($multiple_cardinality) {
            if (!isset($parent->objects[$type])) {
              $parent->objects[$type] = array();
            }

            $parent->objects[$type][$arg] = $object;
          }
          else {
            $parent->objects[$type] = $object;
          }

          if (!$object_is_valid) { // @todo check it.
            break;
          }
        }
      }
      else {
        $object = $multiple_cardinality ? $objects[$type][$arg] : $objects[$type];
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

  public function _getObject($args = NULL) {
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
      // Get current object, which nesting depends on cardinality.
      $class_name = $this->getClassNameByType($type);
      $multiple_cardinality = $class_name::$typeCardinality === OdApiObject::OBJECT_CARDINALITY_IS_MULTIPLE;
      $object = $multiple_cardinality ? $objects[$type][$arg] : $objects[$type];

      // Get children types.
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

    return !empty($object) ? $object : FALSE;
  }

  public function getResult($args) {
    $object = $this->_getObject($args);

    return $object ? $object->getResult() : FALSE;
  }

  public function getContent($args) {
    $object = $this->_getObject($args);

    return $object ? $object->content : FALSE;
  }

  public function getOpendata($site) {
    $args = array(
      'list' => $site,
    );

    $list_result = $this->getResult($args);

    $result = $list_result['meta'];

    foreach ($result as &$meta) {
      $meta_args = $meta['#api'];
      $meta_result = $this->getResult($meta_args);
      $meta = $meta_result;
      unset($meta['#version']);

      $publisher_args = $meta['publisher']['#api'];
      $publisher_result = $this->getResult($publisher_args);
      unset($publisher_result['#version']);
      $meta['publisher'] = $publisher_result;

      $source_args = $meta['source']['#api'];
      $source_result = $this->getResult($source_args);
      unset($meta['source']);
      unset($source_result['#version']);
      $meta['data'] = $source_result['data'];

      foreach ($meta['data'] as &$data) {
        $data_args = $data['#api'];
        $data_result = $this->getResult($data_args);
        unset($data_result['#version']);
        $data = $data_result;

        $structure_args = $data['structure']['#api'];
        $structure_result = $this->getResult($structure_args);
        unset($structure_result['#version']);
        $data['structure'] = $structure_result;
      }
    }
  }

  private function getClassNameByType($type, $version = NULL) { // @todo Сделать.
    $version = empty($version) ? 'Default' : filter_var($version, FILTER_SANITIZE_NUMBER_INT);
    return 'OdApi' . ucfirst($type) . $version;
  }
}
