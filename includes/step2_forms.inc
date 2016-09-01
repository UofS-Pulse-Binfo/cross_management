<?php

/**
 * @file
 * Forms relating to Step #2 of crossing block management.
 */

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form().
 */
function cross_manage_crossingblock_set_marker_data_form($form, $form_state) {

  $markers = array(
    'marker1' => array('A','T','AT'),
    'marker2' => array('absent','present'),
    'marker3' => array('AATA','AATT','TTAT'),
  );

  $lines_by_type = array(
    'Parents' => array(
      'P1','P2','P3','P4','P5','P6','P7','P8','P9','P10','P11','P12','P13','P14','P15','P16'
    ),
    'F1s' => array(
      '2604S','2605S','2606S','2607aS','2608S','2610bS','2613bS',
      '2615S','2616S','2617S','2618S','2619S','2620bS','2621S','2622S'
    ),
  );

  $form['markers'] = array(
    '#type' => 'fieldset',
    '#title' => 'Marker Data',
  );

  foreach ($lines_by_type as $type => $lines) {

    $form['markers'][$type] = array(
      '#type' => 'fieldset',
      '#title' => $type,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#theme' => 'cross_manage_crossingblock_marker_entry'
    );

    foreach ($lines as $line_id) {

      $form['markers'][$type][$line_id] = array(
        '#type' => 'markup',
      );

      $form['markers'][$type][$line_id]['id'] = array(
        '#type' => 'item',
        '#markup' => $line_id
      );

      foreach ($markers as $marker_id => $marker_options_raw) {

        $marker_options = array_combine($marker_options_raw, $marker_options_raw);
        $marker_options[0] = ' ';

        $form['markers'][$type][$line_id][$marker_id] = array(
          '#type' => 'select',
          //'#title' => $line_id .' by '. $marker_id,
          '#options' => $marker_options,
          '#default_value' => 0
        );
      }

    }
  }

  return $form;
}

/**
 * MOCK-UP Form allowing breeders to set the crosses to be made
 * for a given crossing block.
 *
 * Implements hook_form_submit().
 */
function cross_manage_crossingblock_set_marker_data_form_submit($form, $form_state) {

}
