<?php

/**
 * @file
 * Theme & Preprocessor functions for Cross Management
 */

/**
 *
 */
function theme_cross_manage_shepherd_start_tour_link() {

  $textual_link = l(
      'Start Walk-through',
      '',
      array(
        'fragment' => 'cross_manage_crossingblock_f1_form top',
        'attributes' => array('class' => array('tour-start-link', 'tour-textual-link'))
      )
    );
  $image = theme('image', array(
    'path' => drupal_get_path('module', 'cross_manage') . '/theme/images/question-mark-button-green.png',
    'alt' => 'Start Walk-through',
    'title' => 'Start Walk-through'
  ));
  $image_link = l(
    $image,
    '',
    array(
      'fragment' => 'cross_manage_crossingblock_f1_form top',
      'attributes' => array('class' => array('tour-start-link', 'tour-image-link')),
      'html' => TRUE
    )
  );

  return $image_link . $textual_link;
}

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

  $header1 = array(
    'mat' => array('data' => 'Maternal Parent', 'colspan' => 3, 'style' => 'text-align:center', 'class' => 'header-category'),
    'pat' => array('data' => 'Paternal Parent', 'class' => 'header-category'),
    'notes' => array('data' => '', 'class' => 'notes'),
  );
  $header2 = array(
    'pnum' => array('data' => 'Parent Number', 'class' => 'pnum', 'header' => TRUE),
    'line' => array('data' => 'Line', 'class' => 'line', 'header' => TRUE),
    'source' => array('data' => 'Source', 'class' => 'source', 'header' => TRUE),
    // @TODO: Include marker data/selection
    //'marker_data' => array('data' => 'Markers', 'class' => 'marker_data', 'header' => TRUE),
    'cross_to' => array('data' => 'Cross To', 'class' => 'cross_to', 'header' => TRUE),
    'notes' => array('data' => 'Notes', 'class' => 'notes', 'header' => TRUE),
  );

  $rows = array();
  $rows[] = $header2;
  foreach (element_children($element) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($header2 as $fieldname => $title) {
      $cell = array(
        'data' => drupal_render($element[$key][$fieldname]),
        'class' => array('cross-parents', 'parent-cell', $fieldname),
      );

      $row['data'][] = $cell;
      $row['class'] = array('cross-parents-row', 'draggable', $element[$key]['highlight_class']['#value']); // needed for table dragging
      if (sizeof($rows) == 0) {
        $row['class'][] = 'first';
      }
    }
    $rows[] = $row;
  }

  end($rows);
  $last_key = key($rows);
  $rows[$last_key]['class'][] = 'last';
  $rows[$last_key]['class'][] = 'add-new';

  return theme('table', array(
    'header' => $header1,
    'rows' => $rows,
    'attributes' => array('id' => 'cross_manage_crossingblock_parents_form', 'class' => array('cross-manage','cross-manage-table-form','step1')), // needed for table dragging
  ));
}

/**
 * Theme the F1's Form table
 */
function theme_cross_manage_crossingblock_f1($vars) {
  $element = $vars['element'];

  $header = array(
    'cross_num' => array('data' => 'Cross Number', 'class' => 'cross_num'),
    'type' => array('data' => 'Type', 'class' => 'type'),
    'seed_num' => array('data' => 'Seed Produced', 'class' => 'seed_num'),
    'seed_allocate' => array('data' => 'Seed Allocation', 'class' => 'seed_allocate'),
    // @TODO: Include marker data/selection
    //'marker_data' => array('data' => 'Markers', 'class' => 'markers'),
    'cross_to' => array('data' => 'Cross To', 'class' => 'cross_to'),
    'notes' => array('data' => 'Notes', 'class' => 'notes'),

  );

  $rows = array();

  foreach (element_children($element) as $key) {
      $row = array();
      $row['data'] = array();
      foreach ($header as $fieldname => $title) {
        $cell = array(
          'data' => drupal_render($element[$key][$fieldname]),
          'class' => array('cross-parents', 'parent-cell', $fieldname),
          'id' => 'f1-row' . $key . '-'. $fieldname
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
    'attributes' => array('id' => 'cross_manage_crossingblock_f1_form', 'class' => array('cross-manage','cross-manage-table-form','step1')), // needed for table dragging
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
    'attributes' => array('id' => 'cross_manage_crossingblock_marker_form', 'class' => array('cross-manage','cross-manage-table-form','step1')), // needed for table dragging
  )) . drupal_render($element['add-row']);
}

/**
 * Theme the Register Crosses Recentaly added table.
 */
function theme_cross_manage_crossingblock_register_crosses($vars) {
  $element = $vars['element'];
  
  $header = array(
    'cross_num' => array('data' => 'Cross Number', 'class' => 'cross_num_col', 'fields' => array('cross_num')),
    'type' => array('data' => 'Resulting Cross', 'class' => 'type', 'fields' => array('type', 'species')),
    'parent' => array('data' => 'Parents', 'class' => 'parents', 'fields' => array('maternal_parent_icon', 'maternal_parent', 'paternal_parent_icon', 'paternal_parent')),
    'data' => array('data' => 'Characteristics', 'class' => 'data', 'fields' => array('data')),
    'notes' => array('data' => 'Notes', 'class' => 'notes', 'fields' => array('notes')),
    'action' => array('data' => '', 'class' => 'action', 'fields' => array('action')),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();
    $row['data'] = array();
    foreach ($header as $columnname => $data) {
      
      $rendered = '';
      foreach ($data['fields'] as $fieldname) {
        $rendered .= drupal_render($element[$key][$fieldname]);
      }
      
      $cell = array(
        'data' => $rendered,
        'class' => array('register-crosses', 'cross-cell', $columnname),
      );

      $row['data'][] = $cell;
      $row['class'] = array('register-crosses-row', 'draggable'); // needed for table dragging
      if (sizeof($rows) == 0) {
        $row['class'][] = 'first';
      }
    }
    $rows[] = $row;
  }

  $rows[0]['class'][] = 'add-new';
  
  end($rows);
  $last_key = key($rows);
  $rows[$last_key]['class'][] = 'last';

  return theme('table', array(
    'caption' => t('Registered Crosses'),
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'cross_manage_crossingblock_register_crosses_form', 'class' => array('cross-manage','cross-manage-table-form','step4')), // needed for table dragging
  ));
}

/**
 * Theme the Register Crosses Add new table.
 */
function theme_cross_manage_crossingblock_register_crosses_add_new($vars) {
  $element = $vars['element'];
  
  $header = array(
    'cross_num' => array('data' => 'Cross Number', 'class' => 'cross_num_col', 'fields' => array('cross_num_col')),
    'type' => array('data' => 'Resulting Cross', 'class' => 'type', 'fields' => array('type', 'species')),
    'parent' => array('data' => 'Parents', 'class' => 'parents', 'fields' => array('maternal_parent_icon', 'maternal_parent', 'paternal_parent_icon', 'paternal_parent')),
    'data' => array('data' => 'Characteristics', 'class' => 'data', 'fields' => array('data')),
    'notes' => array('data' => 'Notes', 'class' => 'notes', 'fields' => array('notes')),
  );

  $rows = array();
  $row = array();
  $row['data'] = array();
  foreach ($header as $columnname => $data) {
    
    $rendered = '';
    foreach ($data['fields'] as $fieldname) {
      $rendered .= drupal_render($element[$fieldname]);
    }
    
    $cell = array(
      'data' => $rendered,
      'class' => array('register-crosses', 'cross-cell', $columnname),
    );

    $row['data'][] = $cell;
    $row['class'] = array('register-crosses-row', 'draggable'); // needed for table dragging
    if (sizeof($rows) == 0) {
      $row['class'][] = 'first';
    }
  }
  $rows[] = $row;

  end($rows);
  $last_key = key($rows);
  $rows[$last_key]['class'][] = 'last';
  $rows[$last_key]['class'][] = 'add-new';

  return theme('table', array(
    'caption' => t('Register New Cross'),
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'cross_manage_crossingblock_register_crosses_form_add_new', 'class' => array('cross_manage_crossingblock_register_crosses_form', 'cross-manage','cross-manage-table-form','step4')), // needed for table dragging
  )) . render($element['add-row']);
}