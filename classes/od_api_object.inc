<?php

/**
 * @file
 * Contains the OdApiObject class.
 */

abstract class OdApiObject {
  const OBJECT_TYPE_IS_LIST = 'list';
  const OBJECT_TYPE_IS_META = 'meta';
  const OBJECT_TYPE_IS_PUBLISHER = 'publisher';
  const OBJECT_TYPE_IS_SOURCE = 'source';
  const OBJECT_TYPE_IS_DATA = 'data';
  const OBJECT_TYPE_IS_STRUCTURE = 'structure';

  const OBJECT_CARDINALITY_IS_SINGLE = 1;
  const OBJECT_CARDINALITY_IS_MULTIPLE = 2;

  const MESSAGE_TYPE_IS_STATUS = 1;
  const MESSAGE_TYPE_IS_WARNING = 2;
  const MESSAGE_TYPE_IS_ERROR = 3;

  const MESSAGE_TYPE_IS_CRITICAL = 1;
  const MESSAGE_TYPE_IS_NOT_CRITICAL = 2;

  // Errors.
  const MACHINE_FORMAT_DETECTED = 1001;

  // Warnings.
  const FORMAT_MICRODATA_DETECTED = 2031;
  const FORMAT_RDFA_DETECTED = 2041;
  const FORMAT_CSV_DETECTED = 2051;
  const FORMAT_CSV_IS_UTF8 = 2052;
  const FORMAT_XML_DETECTED = 2061;
  const FORMAT_JSON_DETECTED = 2071;

  protected static $type;
  protected static $parentType;
  /**
   * @var array
   */
  protected static $childrenTypes;
  /**
   * @var array
   */
  protected static $message_texts;

  public static $title;

  private $localNamespaces; // @todo Check it.

  /**
   * Root opendata api object.
   *
   * @var OdApi
   */
  public $od_api;

  // Object api args.
  public $args;
  public $api;

  /**
   * Object parent.
   *
   * @var OdApiObject
   */
  public $parent;
  public $objects;

  // Defined parser type.
  public $formatType;

  // Defined urls.
  public $url;
  public $urls;

  /**
   * RDFa parser.
   *
   * @var OdApiRDFaParser
   */
  public $parserRDFa;
  public $parserMicrodata;
  /**
   * CSV parser.
   *
   * @var OdApiCSVParser
   */
  public $parserCSV;
  public $parserXML;
  public $parserJSON;

  // Messages.
  public $messages = array();

  // Result.
  public $content;
  public $result;

  // Validation status variables.
  public $argIsValid = TRUE;
  public $versionIsValid = TRUE;
  public $objectIsValid = TRUE;

  abstract public function setPossibleUrls();
  abstract public function validateArg();
  abstract public function validateVersion();
  abstract public function validateObject();
  abstract public function _parseRdf();
  abstract public function _parseCsv();
  abstract public function getResult();

  /**
   * @param $od_api OdApi
   * @param $parent OdApiObject
   */
  public function __construct($od_api, $parent) {
    $this->od_api = $od_api;

    if ($this::$parentType) {
      if ($parent && is_object($parent) && $parent::$type == $this::$parentType) {
        $this->parent = $parent;
      }
    }

    $this->args = $od_api->currentArgs;

    $this->validateArg();
    $this->argIsValid = !$this->checkErrors();
    if (!$this->argIsValid) {
      return;
    }

    $this->validateVersion();
    $this->versionIsValid = !$this->checkErrors();

    if (!$this->versionIsValid) {
      return;
    }

    $this->api = $this->getApi();

    $this->setLocalNamespaces();
    $this->setPossibleUrls();

    $this->validateObject();
    $this->objectIsValid = !$this->checkErrors();

    if (!$this->objectIsValid) {
      return;
    }

    $this->parse();
    $this->objectIsValid = !$this->checkErrors();

    if (!$this->objectIsValid) {
      return;
    }
  }

  private function setLocalNamespaces() {
    $local_namespaces = array(
      'rdf:' => 'http://www.w3.org/1999/02/22-rdf-syntax-ns#',
      'dc:' => 'http://purl.org/dc/terms/',
      'foaf:' => 'http://xmlns.com/foaf/0.1/',
    );

    $this->localNamespaces = $local_namespaces;
  }

  public function getApi() {
    $args = func_get_args();

    $api = $this->args;
    if ($args) {
      foreach ($args as $arg) {
        $api = array_merge($api, $arg);
      }
    }

    return $api;
  }

  public function localNamespaceWrap($var) {
    return strtr($var, array_flip($this->localNamespaces));
  }

  public function localNamespaceUnwrap($var) {
    return strtr($var, $this->localNamespaces);
  }

  public function parse() {
    if (isset($this->urls['semantic']) && $this->parseRDFa()) {
      $this->formatType = 'rdfa';
    }
    else if (isset($this->urls['file_csv']) && $this->parseCSV()) {
      $this->formatType = 'csv';
    }

    $this->setMessage($this::MACHINE_FORMAT_DETECTED, !empty($this->formatType));
  }

  public function parseRDFa() {
    $this->parserRDFa = new OdApiRDFaParser();
    $this->parserRDFa->parse($this->urls['semantic']);

    $result = $this->_parseRdf();

    $this->setMessage($this::FORMAT_RDFA_DETECTED, $result);

    return $result;
  }

  public function parseCSV() {
    $this->parserCSV = new OdApiCSVParser();
    $this->parserCSV->parse($this->urls['file_csv']);

    $result = $this->_parseCsv();

    $this->setMessage($this::FORMAT_CSV_DETECTED, $result);

    return $result;
  }

  public function getMessageText($message_id, $value, $args) {
    $message_texts = $this::$message_texts;

    $message = isset($message_texts[$message_id]) ? format_string($message_texts[$message_id][$value], $args) : FALSE;

    return $message;
  }

  public function setMessage($message_id, $value, $args = array()) {
    $check_type = (integer) substr($message_id, 0, 1);

    if (!$value) {
      if ($check_type === $this::MESSAGE_TYPE_IS_CRITICAL) {
        $message_type = $this::MESSAGE_TYPE_IS_ERROR;
      }
      else {
        $message_type = $this::MESSAGE_TYPE_IS_WARNING;
      }
    }
    else {
      $message_type = $this::MESSAGE_TYPE_IS_STATUS;
    }

    if (!isset($this->messages[$message_type])) {
      $this->messages[$message_type] = array();
    }

    $this->messages[$message_type][$message_id] = $this->getMessageText($message_id, $value, $args);
  }

  public function getMessages($message_type) {
    $messages = isset($this->messages[$message_type]) ? $this->messages[$message_type] : FALSE;

    return $messages;
  }

  public function checkUrl($url) {
    $curl = curl_init();
    curl_setopt($curl, CURLOPT_URL, $url);
    curl_setopt($curl, CURLOPT_HEADER, 1);
    curl_setopt($curl, CURLOPT_NOBODY, 1);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($curl, CURLOPT_FRESH_CONNECT, 1);

    $result = curl_exec($curl) && curl_getinfo($curl, CURLINFO_HTTP_CODE) < 400;

    curl_close($curl);

    return $result;
  }

  public function checkErrors() {
    return (bool) $this->getMessages($this::MESSAGE_TYPE_IS_ERROR);
  }
}
