<?php

/**
 * @file
 * Defines default views for the cross management system.
 */

/**
 * Implements hook_views_default_views().
 */
function cross_manage_views_default_views() {
  $views = array();
  
  $view = cross_manage_views_default_parent_list();
  $views[$view->name] = $view;

  $view = cross_manage_views_default_seed_list();
  $views[$view->name] = $view;
  
  //$view = cross_manage_views_default_crosses();
  //$views[$view->name] = $view;

  //$view = cross_manage_views_default_growout_list();
  //$views[$view->name] = $view;

  //$view = cross_manage_views_default_progeny_list();
  //$views[$view->name] = $view;

  return $views;
}

/**
 * Parent List
 */
function cross_manage_views_default_parent_list() {

  $view = new view();
  $view->name = 'cb_parent_list';
  $view->description = 'List of parents for a given crossing block.';
  $view->tag = 'Crossing Block';
  $view->base_table = 'cross_manage_parents';
  $view->human_name = 'Crossing Block Parent List';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Parent List';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'parent_number' => 'parent_number',
    'nid' => 'nid',
    'name' => 'name',
    'genus' => 'genus',
    'species' => 'species',
    'nothing_1' => 'nothing_1',
    'seed_source' => 'seed_source',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'parent_number' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'genus' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'species' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'seed_source' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Cross Parents: Chado Stock */
  $handler->display->display_options['relationships']['stock_id']['id'] = 'stock_id';
  $handler->display->display_options['relationships']['stock_id']['table'] = 'cross_manage_parents';
  $handler->display->display_options['relationships']['stock_id']['field'] = 'stock_id';
  /* Field: Cross Parents: Parent Number */
  $handler->display->display_options['fields']['parent_number']['id'] = 'parent_number';
  $handler->display->display_options['fields']['parent_number']['table'] = 'cross_manage_parents';
  $handler->display->display_options['fields']['parent_number']['field'] = 'parent_number';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Field: Chado Stock: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'stock';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['name']['label'] = 'Parent Name';
  $handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['name']['alter']['path'] = 'node/[nid]';
  /* Field: Chado Organism: Genus */
  $handler->display->display_options['fields']['genus']['id'] = 'genus';
  $handler->display->display_options['fields']['genus']['table'] = 'organism';
  $handler->display->display_options['fields']['genus']['field'] = 'genus';
  $handler->display->display_options['fields']['genus']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['genus']['exclude'] = TRUE;
  /* Field: Chado Organism: Species */
  $handler->display->display_options['fields']['species']['id'] = 'species';
  $handler->display->display_options['fields']['species']['table'] = 'organism';
  $handler->display->display_options['fields']['species']['field'] = 'species';
  $handler->display->display_options['fields']['species']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['species']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = 'Parent Species';
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = '[genus] [species]';
  /* Field: Cross Parents: Seed Source */
  $handler->display->display_options['fields']['seed_source']['id'] = 'seed_source';
  $handler->display->display_options['fields']['seed_source']['table'] = 'cross_manage_parents';
  $handler->display->display_options['fields']['seed_source']['field'] = 'seed_source';
  /* Sort criterion: Cross Parents: Crossing Block Year */
  $handler->display->display_options['sorts']['cb_year']['id'] = 'cb_year';
  $handler->display->display_options['sorts']['cb_year']['table'] = 'cross_manage_parents';
  $handler->display->display_options['sorts']['cb_year']['field'] = 'cb_year';
  $handler->display->display_options['sorts']['cb_year']['order'] = 'DESC';
  /* Sort criterion: Cross Parents: Crossing Block Season */
  $handler->display->display_options['sorts']['cb_season']['id'] = 'cb_season';
  $handler->display->display_options['sorts']['cb_season']['table'] = 'cross_manage_parents';
  $handler->display->display_options['sorts']['cb_season']['field'] = 'cb_season';
  /* Sort criterion: Cross Parents: Crossing Block Genus */
  $handler->display->display_options['sorts']['cb_species']['id'] = 'cb_species';
  $handler->display->display_options['sorts']['cb_species']['table'] = 'cross_manage_parents';
  $handler->display->display_options['sorts']['cb_species']['field'] = 'cb_species';
  /* Sort criterion: Cross Parents: Parent Number */
  $handler->display->display_options['sorts']['parent_number']['id'] = 'parent_number';
  $handler->display->display_options['sorts']['parent_number']['table'] = 'cross_manage_parents';
  $handler->display->display_options['sorts']['parent_number']['field'] = 'parent_number';
  /* Contextual filter: Cross Parents: Crossing Block Genus */
  $handler->display->display_options['arguments']['cb_species']['id'] = 'cb_species';
  $handler->display->display_options['arguments']['cb_species']['table'] = 'cross_manage_parents';
  $handler->display->display_options['arguments']['cb_species']['field'] = 'cb_species';
  $handler->display->display_options['arguments']['cb_species']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['cb_species']['title_enable'] = TRUE;
  $handler->display->display_options['arguments']['cb_species']['title'] = 'Parents: %1, %3 %2 Crossing Block';
  $handler->display->display_options['arguments']['cb_species']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['cb_species']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['cb_species']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['cb_species']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['cb_species']['limit'] = '0';
  /* Contextual filter: Cross Parents: Crossing Block Year */
  $handler->display->display_options['arguments']['cb_year']['id'] = 'cb_year';
  $handler->display->display_options['arguments']['cb_year']['table'] = 'cross_manage_parents';
  $handler->display->display_options['arguments']['cb_year']['field'] = 'cb_year';
  $handler->display->display_options['arguments']['cb_year']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['cb_year']['title_enable'] = TRUE;
  $handler->display->display_options['arguments']['cb_year']['title'] = 'Parents: %1, %3 %2 Crossing Block';
  $handler->display->display_options['arguments']['cb_year']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['cb_year']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['cb_year']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['cb_year']['summary_options']['items_per_page'] = '25';
  /* Contextual filter: Cross Parents: Crossing Block Season */
  $handler->display->display_options['arguments']['cb_season']['id'] = 'cb_season';
  $handler->display->display_options['arguments']['cb_season']['table'] = 'cross_manage_parents';
  $handler->display->display_options['arguments']['cb_season']['field'] = 'cb_season';
  $handler->display->display_options['arguments']['cb_season']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['cb_season']['title_enable'] = TRUE;
  $handler->display->display_options['arguments']['cb_season']['title'] = 'Parents: %1, %3 %2 Crossing Block';
  $handler->display->display_options['arguments']['cb_season']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['cb_season']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['cb_season']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['cb_season']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['cb_season']['limit'] = '0';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'crossing-block/%/%/%/parent-list';

  return $view;
}

/**
 * Seed List
 */
function cross_manage_views_default_seed_list() {

  $view = new view();
  $view->name = 'cb_seed_list';
  $view->description = 'Listing of seed sources for parents.';
  $view->tag = 'Crossing Block';
  $view->base_table = 'cross_manage_parents';
  $view->human_name = 'Seed List';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Seed List';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'seed_source',
      'rendered' => 0,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['columns'] = array(
    'seed_source' => 'seed_source',
    'parent_number' => 'parent_number',
    'name' => 'name',
    'uniquename' => 'uniquename',
    'species' => 'species',
    'genus' => 'genus',
    'nothing' => 'nothing',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'seed_source' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'parent_number' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'uniquename' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'species' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'genus' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: Cross Parents: Chado Stock */
  $handler->display->display_options['relationships']['stock_id']['id'] = 'stock_id';
  $handler->display->display_options['relationships']['stock_id']['table'] = 'cross_manage_parents';
  $handler->display->display_options['relationships']['stock_id']['field'] = 'stock_id';
  /* Field: Cross Parents: Seed Source */
  $handler->display->display_options['fields']['seed_source']['id'] = 'seed_source';
  $handler->display->display_options['fields']['seed_source']['table'] = 'cross_manage_parents';
  $handler->display->display_options['fields']['seed_source']['field'] = 'seed_source';
  $handler->display->display_options['fields']['seed_source']['exclude'] = TRUE;
  /* Field: Cross Parents: Parent Number */
  $handler->display->display_options['fields']['parent_number']['id'] = 'parent_number';
  $handler->display->display_options['fields']['parent_number']['table'] = 'cross_manage_parents';
  $handler->display->display_options['fields']['parent_number']['field'] = 'parent_number';
  /* Field: Chado Stock: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'stock';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['name']['label'] = 'Parent Name';
  /* Field: Chado Stock: Uniquename */
  $handler->display->display_options['fields']['uniquename']['id'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['table'] = 'stock';
  $handler->display->display_options['fields']['uniquename']['field'] = 'uniquename';
  $handler->display->display_options['fields']['uniquename']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['uniquename']['label'] = 'Parent Accession';
  /* Field: Chado Organism: Species */
  $handler->display->display_options['fields']['species']['id'] = 'species';
  $handler->display->display_options['fields']['species']['table'] = 'organism';
  $handler->display->display_options['fields']['species']['field'] = 'species';
  $handler->display->display_options['fields']['species']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['species']['exclude'] = TRUE;
  /* Field: Chado Organism: Genus */
  $handler->display->display_options['fields']['genus']['id'] = 'genus';
  $handler->display->display_options['fields']['genus']['table'] = 'organism';
  $handler->display->display_options['fields']['genus']['field'] = 'genus';
  $handler->display->display_options['fields']['genus']['relationship'] = 'stock_id';
  $handler->display->display_options['fields']['genus']['exclude'] = TRUE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = 'Species';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '[genus] [species]';
  /* Sort criterion: Cross Parents: Seed Source */
  $handler->display->display_options['sorts']['seed_source']['id'] = 'seed_source';
  $handler->display->display_options['sorts']['seed_source']['table'] = 'cross_manage_parents';
  $handler->display->display_options['sorts']['seed_source']['field'] = 'seed_source';
  /* Sort criterion: Chado Stock: Name */
  $handler->display->display_options['sorts']['name']['id'] = 'name';
  $handler->display->display_options['sorts']['name']['table'] = 'stock';
  $handler->display->display_options['sorts']['name']['field'] = 'name';
  $handler->display->display_options['sorts']['name']['relationship'] = 'stock_id';
  /* Contextual filter: Cross Parents: Crossing Block Genus */
  $handler->display->display_options['arguments']['cb_species']['id'] = 'cb_species';
  $handler->display->display_options['arguments']['cb_species']['table'] = 'cross_manage_parents';
  $handler->display->display_options['arguments']['cb_species']['field'] = 'cb_species';
  $handler->display->display_options['arguments']['cb_species']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['cb_species']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['cb_species']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['cb_species']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['cb_species']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['cb_species']['limit'] = '0';
  /* Contextual filter: Cross Parents: Crossing Block Year */
  $handler->display->display_options['arguments']['cb_year']['id'] = 'cb_year';
  $handler->display->display_options['arguments']['cb_year']['table'] = 'cross_manage_parents';
  $handler->display->display_options['arguments']['cb_year']['field'] = 'cb_year';
  $handler->display->display_options['arguments']['cb_year']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['cb_year']['summary']['format'] = 'default_summary';
  /* Contextual filter: Cross Parents: Crossing Block Season */
  $handler->display->display_options['arguments']['cb_season']['id'] = 'cb_season';
  $handler->display->display_options['arguments']['cb_season']['table'] = 'cross_manage_parents';
  $handler->display->display_options['arguments']['cb_season']['field'] = 'cb_season';
  $handler->display->display_options['arguments']['cb_season']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['cb_season']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['cb_season']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['cb_season']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['cb_season']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['cb_season']['limit'] = '0';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'crossing-block/%/%/%/seed-list';

  return $view;
}

/**
 * Crosses
 */
function cross_manage_views_default_crosses() {

    $view = new view();
    
    return $view;
}

/**
 * Grow-outs List
 */
function cross_manage_views_default_growout_list() {

    $view = new view();
    
    return $view;
}

/**
 * Progeny List
 */
function cross_manage_views_default_progeny_list() {

    $view = new view();
    
    return $view;
}