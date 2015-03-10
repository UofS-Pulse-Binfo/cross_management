<?php

/**
 * @file
 * Theme & Preprocessor functions for Cross Management
 */

/**
 * Proprocessor: cross_manage_workflow.tpl.php
 */
function cross_manage_preprocess_cross_manage_workflow(&$vars) {
  $path = drupal_get_path('module', 'cross_manage');

  // Module-specific CSS
  drupal_add_css($path . '/theme/css/cross_manage.css');

  // These files are needed to get the accordion list to work.
  if ($jquery_path = libraries_get_path('jquery_ui')) {
    drupal_add_css($jquery_path . '/css/kptheme/jquery-ui-1.10.4.custom.css');
    drupal_add_js($jquery_path . '/js/jquery-1.10.2.js');
    drupal_add_js($jquery_path . '/js/jquery-ui-1.10.4.custom.js');
  }

  // Paths to the various forms to be linked to from the workflow.
  $vars['form_paths']['parents'] = 'crossing-block/@species/@year/@season/set-crosses/parents';
  $vars['form_paths']['F1'] = 'crossing-block/@species/@year/@season/set-crosses/F1s';
  $vars['form_paths']['marker'] = 'crossing-block/@species/@year/@season/set-crosses/marker-data';
  $vars['form_paths']['progeny'] = 'crossing-block/@species/@year/@season/report-crosses';
  // Replace placeholders in paths.
  foreach($vars['form_paths'] as $k => $path_string) {
    $vars['form_paths'][$k] = format_string($path_string, array(
      '@species' => $vars['crossingblock_species'],
      '@year' => $vars['crossingblock_year'],
      '@season' => $vars['crossingblock_season'],
    ));
  }

  // Set the steps based on where we are in the workflow.
  // @TODO: Make dynamic.
  $vars['form_steps']['step1']['complete'] = TRUE;
  $vars['form_steps']['step2']['complete'] = TRUE;
  $vars['form_steps']['step3']['complete'] = TRUE;
  $vars['form_steps']['step4']['complete'] = FALSE;
  $vars['form_steps']['step5']['complete'] = FALSE;

  $vars['form_steps']['active_panel'] = 0;

}

/**
 * Theme the Parents Form table
 */
function theme_cross_manage_crossingblock_parents($vars) {
  $element = $vars['element'];

  $header = array(
    'pnum' => array('data' => 'Parent Number', 'class' => 'pnum'),
    'line' => array('data' => 'Line', 'class' => 'line'),
    'source' => array('data' => 'Source', 'class' => 'source'),
    // @TODO: Include marker data/selection
    //'marker_data' => array('data' => 'Markers', 'class' => 'marker_data'),
    'notes' => array('data' => 'Notes', 'class' => 'notes'),
    'cross_to' => array('data' => 'Cross To', 'class' => 'cross_to'),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    if (preg_match('/^[0-9]+$/',$key)) {
      $row = array();
      $row['data'] = array();
      foreach ($header as $fieldname => $title) {
        $cell = array(
          'data' => drupal_render($element[$key][$fieldname]),
          'class' => array('cross-parents', 'parent-cell', $fieldname),
        );

        $row['data'][] = $cell;
        $row['class'] = array('cross-parents-row', 'draggable', $element[$key]['highlight_class']['#value']); // needed for table dragging
      }
      $rows[] = $row;
    }
  }

  end($rows);
  $last_key = key($rows);
  $rows[$last_key]['class'][] = 'last';

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'cross_manage_crossingblock_parents_form'), // needed for table dragging
  )) . drupal_render($element['add-row']);
}

/**
 * Theme the F1's Form table
 */
function theme_cross_manage_crossingblock_f1($vars) {
  $element = $vars['element'];

  $header = array(
    'cross_num' => array('data' => 'Cross Number', 'class' => 'cross_num'),
    'type' => array('data' => 'Type', 'class' => 'type'),
    'pedigree' => array('data' => 'Pedigree', 'class' => 'pedigree'),
    'seed_num' => array('data' => 'Seed Count', 'class' => 'seed_num'),
    'seed_allocate' => array('data' => 'Seed Allocation', 'class' => 'seed_allocate'),
    //'seed2cross' => array('data' => 'Seed to Cross', 'class' => 'seed2cross'),
    //'seed2reserve' => array('data' => 'Seed to Reserve', 'class' => 'seed2reserve'),
    //'seed2growout' => array('data' => 'Seed to Grow-out', 'class' => 'seed2growout'),
    // @TODO: Include marker data/selection
    //'marker_data' => array('data' => 'Markers', 'class' => 'markers'),
    'notes' => array('data' => 'Notes', 'class' => 'notes'),
    'cross_to' => array('data' => 'Cross To', 'class' => 'cross_to'),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
      $row = array();
      $row['data'] = array();
      foreach ($header as $fieldname => $title) {
        $cell = array(
          'data' => drupal_render($element[$key][$fieldname]),
          'class' => array('cross-parents', 'parent-cell', $fieldname),
        );

        $row['data'][] = $cell;
        $row['class'] = array('cross-parents-row', 'draggable'); // needed for table dragging
      }
      $rows[] = $row;
  }

  end($rows);
  $last_key = key($rows);
  $rows[$last_key]['class'][] = 'last';

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'cross_manage_crossingblock_f1_form'), // needed for table dragging
  )) . drupal_render($element['add-row']);
}

/**
 * Theme the Marker's Form table
 */
function theme_cross_manage_crossingblock_marker_entry($vars) {
  $element = $vars['element'];

  $header = array(
    'id' => array('data' => 'Line', 'class' => 'line'),
    'marker1' => array('data' => 'Marker1', 'class' => 'marker1'),
    'marker2' => array('data' => 'Marker2', 'class' => 'marker2'),
    'marker3' => array('data' => 'Marker3', 'class' => 'marker3'),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
      $row = array();
      $row['data'] = array();
      foreach ($header as $fieldname => $title) {
        $cell = array(
          'data' => drupal_render($element[$key][$fieldname]),
          'class' => array('cross-parents', 'parent-cell', $fieldname),
        );

        $row['data'][] = $cell;
        $row['class'] = array('cross-parents-row', 'draggable'); // needed for table dragging
      }
      $rows[] = $row;
  }

  end($rows);
  $last_key = key($rows);
  $rows[$last_key]['class'][] = 'last';

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'cross_manage_crossingblock_f1_form'), // needed for table dragging
  )) . drupal_render($element['add-row']);
}

