<?php

/**
 * @file
 * Contains the OdApiRDFParser class.
 */

/**
 * Opendata API class.
 */
class OdApiCSVParser extends OdApiParser {
  public function __construct() {
    return $this;
  }

  public function parse($url) {
    // $this->comments = t('Пров', 'status');
    $this->url = $url;

    $header = array();
    $header[] = 'Cache-Control: private, no-cache, no-store, must-revalidate, max-age=0';
    $header[] = 'Pragma: no-cache';

    $opts = array(
      'http' => array(
        'method'  => 'POST',
        'header'  => implode("\n", $header),
      ),
    );

    $context = stream_context_create($opts);

    $step = array(
      'title' => 'Проверяем наличие csv файла',
    );
    $file = @fopen($url, 'r', NULL, $context);
    if ($file === FALSE) {
      $this->error = 'fileNotExists';
      return $this;
    }

    $delimiters = array(',', ';');
    foreach ($delimiters as $delimiter) {
      $columns_count = NULL;
      while (($csv_str = fgetcsv($file, 0, $delimiter)) !== FALSE) {
        if ($columns_count === NULL) {
          $columns_count = count($csv_str);
          if (!in_array($columns_count, range(2, 6))) {
            $this->error = 'fileNotExists';
            break;
          }
          foreach ($csv_str as $str) {
            if (mb_strlen($str) > 64) {
              $this->error = 'fileNotExists';
              break(2);
            }
          }
        }
        else {
          if (count($csv_str) !== $columns_count) {
            $this->error = 'fileNotExists';
            break;
          }
        }

        // Check UTF-8 encoding.
        if (!preg_match('//u', implode(',', $csv_str))) {
          $this->error = 'notUtf8';
          break;
        }
        $this->result[] = $csv_str;
      }
      fclose($file);

      if (empty($this->error)) {
        break;
      }
      else {
        rewind($file);
      }
    }

    return $this;
  }

  public function getResult() {
    return $this->result;
  }

  public function validate() {

  }
}
