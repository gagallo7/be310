<?php
/**
 * @file
 * Spider Catalog module global options.
 */

/**
 * Menu loader callback. Load global options for the catalog.
 */
function spider_catalog_global_options() {
  if (db_query("SELECT * FROM {spider_catalog_params}")->fetchObject()) {
    $row = db_query("SELECT * FROM {spider_catalog_params}")->fetchObject();
    $category_details_width = $row->category_details_width;
    $price = $row->price;
    $market_price = $row->market_price;
    $currency_symbol = $row->currency_symbol;
    $currency_symbol_position = $row->currency_symbol_position;
    $product_ratings = $row->product_ratings;
    $customer_reviews = $row->customer_reviews;
    $search_by_category = $row->search_by_category;
    $search_by_name = $row->search_by_name;
  }
  $form = array();
  $free_version = '
    <div class="messages warning">Global Options are disabled in free version. If you need this functionality, you need to buy the commercial version.</div>
    <a href="http://web-dorado.com/products/drupal-catalog.html" target="_blank" style="color:red; text-decoration:none; float:right;">
                    <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/header.png" border="0" alt="www.web-dorado.com" width="215"><br />
                  <div style="float:right;">' . t('Get the full version') . '&nbsp;&nbsp;&nbsp;&nbsp;</div>
                  </a>';
  $form['fieldset_global_options'] = array(
    '#prefix' => $free_version,
    '#type' => 'fieldset',
    '#title' => t('Global Options'),
  );
  $form['fieldset_global_options']['category_details_width'] = array(
    '#type' => 'textfield',
    '#description' => t('Category details width in pixels.'),
    '#title' => t('Category Details Width'),
    '#default_value' => $category_details_width,
    '#size' => 5,
  );
  $form['fieldset_global_options']['price'] = array(
    '#type' => 'radios',
    '#description' => t('Show or hide price.'),
    '#title' => t('Price'),
    '#default_value' => $price,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['fieldset_global_options']['market_price'] = array(
    '#type' => 'radios',
    '#description' => t('Show or hide market price.'),
    '#title' => t('Market Price'),
    '#default_value' => $market_price,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['fieldset_global_options']['currency_symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency Symbol'),
    '#default_value' => $currency_symbol,
    '#size' => 5,
  );
  $form['fieldset_global_options']['currency_symbol_position'] = array(
    '#type' => 'radios',
    '#description' => t('Currency Symbol Position (before or after number).'),
    '#title' => t('Currency Symbol Position'),
    '#default_value' => $currency_symbol_position,
    '#options' => array('0' => t('Before'), '1' => t('After')),
  );
  $form['fieldset_global_options']['product_ratings'] = array(
    '#type' => 'radios',
    '#description' => t('Enable or disable Product Ratings.'),
    '#title' => t('Product Ratings'),
    '#default_value' => $product_ratings,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['fieldset_global_options']['customer_reviews'] = array(
    '#type' => 'radios',
    '#description' => t('Enable or disable Customer Reviews.'),
    '#title' => t('Customer Reviews'),
    '#default_value' => $customer_reviews,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['fieldset_global_options']['search_by_category'] = array(
    '#type' => 'radios',
    '#description' => t('Search products by category.'),
    '#title' => t('Search by Category'),
    '#default_value' => $search_by_category,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['fieldset_global_options']['search_by_name'] = array(
    '#type' => 'radios',
    '#description' => t('Search products by name.'),
    '#title' => t('Search by Name'),
    '#default_value' => $search_by_name,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['fieldset_global_options']['global_options_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array('onclick' => 'alert(Drupal.t("You cant save the changes in free version."));return false;'),
  );
  return $form;
}
