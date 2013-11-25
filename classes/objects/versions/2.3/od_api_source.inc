<?php

/**
 * @file
 * Contains the OdApiSource class.
 */

class OdApiSource23 extends OdApiSourceDefault {
  public function validateArg() {
    // Check publisher arg is valid.
    $source_arg = $this->args[OdApiObject::OBJECT_TYPE_IS_SOURCE];
    $source_arg_is_valid = $source_arg === OdApiObject::OBJECT_TYPE_IS_PUBLISHER;
    $this->setValidator($this::SITE_OPENDATA_SOURCE_ARG_IS_VALID, $source_arg_is_valid);
  }

  public function validateVersion() {
    return TRUE;
  }

  public function setPossibleUrls() {
    // Collect available urls.
    $urls = array();

    $dc_source = $this->localNamespaceUnwrap('dc:source');

    if (isset($this->parent->parserRDFa->subjects[$this->parent->url][$dc_source])) {
      $this->url = reset($this->parent->parserRDFa->subjects[$this->parent->url][$dc_source]);
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
      $this->setValidator($this::FORMAT_RDFA_DETECTED, FALSE);
      return FALSE;
    }

    $dc_hasPart = $this->localNamespaceUnwrap('dc:hasPart');
    $dc_created = $this->localNamespaceUnwrap('dc:created');

    $content = array();
    $content['data'] = array();
    if (isset($this->parserRDFa->result[$dc_hasPart])) {
      foreach ($this->parserRDFa->result[$dc_hasPart] as $item) {
        if (isset($this->parserRDFa->subjects[$item][$dc_created])) {
          $created = reset($this->parserRDFa->subjects[$item][$dc_created]);

          $content['data'][] = array(
            'created' => $created,
          );
        }
      }
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
    $content['data'] = array();
    if (isset($csv_content['8']['2'])) {
      $match = preg_match('/^.*data-([0-9-T]+)-structure-[0-9-T]+\.[a-z]*$/u', $csv_content['8']['2'], $matches);
      if ($match && isset($matches['1'])) {
        $version = $matches['1'];
        $content['data'][] = array(
          'version' => $version,
        );
      }
    }

    if (isset($csv_content['16']['2'])) {
      $previous_versions = explode("\n", $csv_content['16']['2']);
      foreach ($previous_versions as $previous_version) {
        $match = preg_match('/^.*data-([0-9-T]+)-structure-[0-9-T]+\.[a-z]*$/u', $previous_version, $matches);
        if ($match && isset($matches['1'])) {
          $version = $matches['1'];
          $content['data'][] = array(
            'version' => $version,
          );
        }
      }
    }

    $this->content = $content;

    return TRUE;
  }

  public function getResult() {
    $result = array(
      '#version' => '2.3',
    );

    $result += $this->content;

    foreach($result['data'] as &$list) {
      if (isset($list['created'])) {
        $date = new DateTime($list['created']);
        $data_arg = $date->format('Y-m-d\TH:i:s');

        $data_api = $this->getApi(array(OdApiObject::OBJECT_TYPE_IS_DATA => $data_arg));

        $version = $data_arg;
      }
      else if (isset($list['version'])) {
        $version = $list['version'];
        $data_api = $this->getApi(array(OdApiObject::OBJECT_TYPE_IS_DATA => $version));

        $version = $list['version'];
      }
      else {
        continue;
      }

      $list = array(
        '#api' => $data_api,
        'version' => $version,
      );
    }

    return $result;
  }
}
