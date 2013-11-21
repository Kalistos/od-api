<?php

/**
 * @file
 * Contains the OdApiPublisher class.
 */

class OdApiPublisher23 extends OdApiPublisherDefault {
  public function validateArg() {
    // Check publisher arg is valid.
    $publisher_arg = $this->args[OdApiObject::OBJECT_TYPE_IS_PUBLISHER];
    $publisher_arg_is_valid = $publisher_arg === OdApiObject::OBJECT_TYPE_IS_PUBLISHER;
    $this->setMessage($this::SITE_OPENDATA_PUBLISHER_ARG_IS_VALID, $publisher_arg_is_valid);
  }

  public function validateVersion() {
    return TRUE;
  }

  public function setPossibleUrls() {
    // Collect available urls.
    $urls = array();

    $dc_publisher = $this->localNamespaceUnwrap('dc:publisher');

    if (isset($this->parent->parserRDFa->subjects[$this->parent->url][$dc_publisher])) {
      $this->url = reset($this->parent->parserRDFa->subjects[$this->parent->url][$dc_publisher]);
      $urls['semantic'] = $this->url;
    }

    $this->urls = $urls;
  }

  public function validateObject() {
    return TRUE;
  }

  public function _parseRdf() {
    // Check RDFa object existing.
    if ($this->parserRDFa->error) {
      $this->setMessage($this::FORMAT_RDFA_DETECTED, FALSE);
      return FALSE;
    }

    $foaf_name = $this->localNamespaceUnwrap('foaf:name');
    $foaf_phone = $this->localNamespaceUnwrap('foaf:phone');
    $foaf_mbox = $this->localNamespaceUnwrap('foaf:mbox');

    $content = array();
    if (isset($this->parserRDFa->result[$foaf_name])) {
      $content['name'] = reset($this->parserRDFa->result[$foaf_name]);
    }
    if (isset($this->parserRDFa->result[$foaf_phone])) {
      $content['phone'] = reset($this->parserRDFa->result[$foaf_phone]);
    }
    if (isset($this->parserRDFa->result[$foaf_mbox])) {
      $content['mbox'] = reset($this->parserRDFa->result[$foaf_mbox]);
    }

    if (!$content) {
      return FALSE;
    }

    $this->content = $content;

    return TRUE;
  }

  public function _parseCsv() {
    return FALSE; // @todo
    if (!$this->parent->csvResult) {
      return FALSE;
    }

    $csv_content = $this->parent->csvResult;

    $content = array();
    if (isset($csv_content['5']['2'])) {
      $content['name'] = $csv_content['5']['2'];
    }
    if (isset($csv_content['6']['2'])) {
      $content['phone'] = $csv_content['6']['2'];
    }
    if (isset($csv_content['7']['2'])) {
      $content['mbox'] = $csv_content['7']['2'];
    }

    if (!$content) {
      return FALSE;
    }

    $this->content = $content;

    return TRUE;
  }

  public function getResult() {
    $result = array(
      '#version' => '2.3',
    );

    $result += $this->content;

    return $result;
  }
}
