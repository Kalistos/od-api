<?php

/**
 * @file
 * Contains the OdApiList class.
 */

class OdApiList23 extends OdApiListDefault {
  public function validateArg() {
    // Add default http:// scheme if it is empty.
    if (empty($this->args[OdApiObject::OBJECT_TYPE_IS_LIST])) {
      $this->setMessage($this::SITE_URL_IS_VALID, FALSE);
      return;
    }

    $arg_list = $this->args[OdApiObject::OBJECT_TYPE_IS_LIST];

    $base_url = $arg_list;
    $base_url = rtrim($base_url, '/') . '/';
    $parsed_url = parse_url($base_url);
    if (!isset($parsed_url['scheme'])) {
      $base_url = 'http://' . $base_url;
    }

    // Define main page url.
    $this->od_api->main_page_url = $base_url;

    // Fix args.
    $this->args[OdApiObject::OBJECT_TYPE_IS_LIST] = $base_url;

    $url_is_valid = (bool) preg_match('/^https?:\/\/[a-zа-я0-9\-]+(\.[a-zа-я0-9\-]+)*\.[a-z]{2,4}\/?$/', $base_url);

    $this->setMessage($this::SITE_URL_IS_VALID, $url_is_valid);
  }

  public function validateVersion() {
    return TRUE;
  }

  public function setPossibleUrls() {
    // Collect available urls.
    $urls = array();

    // Define opendata page url.
    $this->url = $this->od_api->main_page_url . 'opendata';
    $this->od_api->opendata_page_url = $this->url;

    // Visual representation.
    $urls['semantic'] = $this->url;

    // File url template.
    $file_url = $this->url . '/opendatalist.!extension';

    // File CSV representation.
    $urls['file_csv'] = strtr($file_url, array('!extension' => 'csv'));

    // File XML representation.
    $urls['file_xml'] = strtr($file_url, array('!extension' => 'xml'));

    $this->urls = $urls;
  }

  public function validateObject() {
    // Check main page url.
    $site_main_page_exists = $this->checkUrl($this->od_api->main_page_url);
    $this->setMessage($this::SITE_MAIN_PAGE_EXISTS, $site_main_page_exists);

    // Check opendata page url.
    $site_opendata_page_exists = $this->checkUrl($this->url);
    $this->setMessage($this::SITE_OPENDATA_PAGE_EXISTS, $site_opendata_page_exists);

    return $this->checkErrors();
  }

  public function _parseRdf() {
    // Check RDFa object existing.
    if ($this->parserRDFa->error) {
      $this->setMessage($this::FORMAT_RDFA_DETECTED, FALSE);
      return FALSE;
    }

    // Declare used RDFa types.
    $rdf_type = $this->localNamespaceUnwrap('rdf:type');
    $dc_hasPart = $this->localNamespaceUnwrap('dc:hasPart');
    $dc_title = $this->localNamespaceUnwrap('dc:title');
    $dc_Collection = $this->localNamespaceUnwrap('dc:Collection');

    // If object is not DC Collection m
    if (!isset($this->parserRDFa->result[$rdf_type]) || !in_array($dc_Collection, $this->parserRDFa->result[$rdf_type])) {
      $this->setMessage($this::FORMAT_RDFA_DETECTED, FALSE);
      return FALSE;
    }

    $content = array();
    if (!isset($this->parserRDFa->result[$dc_hasPart])) {
      $this->setMessage($this::FORMAT_RDFA_DETECTED, FALSE);
      return FALSE;
    }

    $content['meta'] = array();
    foreach ($this->parserRDFa->result[$dc_hasPart] as $item) {
      if (!isset($this->parserRDFa->subjects[$item][$dc_title])) {
        continue;
      }

      $content['meta'][] = array(
        'title' => reset($this->parserRDFa->subjects[$item][$dc_title]),
        'link' => $item,
      );
    }

    if (!$content) {
      $this->setMessage($this::FORMAT_RDFA_DETECTED, FALSE);
      return FALSE;
    }

    $this->content = $content;

    $this->setMessage($this::FORMAT_RDFA_DETECTED, TRUE);
    return TRUE;
  }

  public function _parseCsv() {
    if (!$this->parserCSV->result) {
      switch ($this->parserCSV->error) {
        case 'fileNotExists':
          $this->setMessage($this::FORMAT_CSV_DETECTED, FALSE);
          break;
        case 'notUtf8':
          $this->setMessage($this::FORMAT_CSV_IS_UTF8, FALSE);
          break;
      }
      return FALSE;
    }

    $csv_content = $this->parserCSV->result;

    $content = array();
    $content['meta'] = array();

    $row_counter = 0;
    foreach ($csv_content as $row) {
      $row_counter++;
      if ($row_counter === 1) {
        continue;
      }

      if (!isset($row['1']) || !isset($row['2'])) {
        continue;
      }
      $content['meta'][] = array(
        'title' => $row['1'],
        'link' => $row['2'],
      );
    }

    $this->content = $content;

    $this->setMessage($this::FORMAT_CSV_DETECTED, TRUE);
    return TRUE;
  }

  public function getResult() {
    $result = array(
      'version' => '2.3',
    );

    $result += $this->content;

    if (!empty($result['meta'])) {
      foreach($result['meta'] as &$list) {
        $identifier = end(explode('/', rtrim($list['link'], '/')));

        $meta_api = $this->getApi(array(OdApiObject::OBJECT_TYPE_IS_META => $identifier));

        $list = array(
          '#api' => $meta_api,
          'identifier' => $identifier,
          'title' => $list['title'],
        );
      }
    }

    return $result;
  }
}
