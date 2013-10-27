<?php
/**
 * @file
 * Star Rating.
 * @author Web-Dorado.
 */

/**
 * Save rating in data base.
 */
function spider_catalog_star_rate() {
  if (isset($_POST['product_id'])) {
    $product_id = $_POST['product_id'];
  }
  else {
    $product_id = 0;
  }
  if (isset($_POST['vote_value'])) {
    $vote_value = $_POST['vote_value'];
  }
  else {
    $vote_value = 0;
  }
  $insert = db_insert('spider_catalog_product_votes')->fields(array(
      'remote_ip' => ip_address(),
      'vote_value' => $vote_value,
      'product_id' => $product_id,
    ))->execute();
  if (!$insert) {
    drupal_add_js('alert(Drupal.t("Error")); window.history.go(-1);', array('type' => 'inline'));
    exit();
  }
  $row = db_query("SELECT AVG(vote_value) as rating FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(
    ':product_id' => $product_id,
    ))->fetchField();
  $rating = drupal_substr($row, 0, 3);
  spider_catalog_print_stars($product_id, $rating);
}

/**
 * Show voting result.
 */
function spider_catalog_print_stars($product_id, $rating) {
  $title = t('Rating') . ' ' . $rating . '&nbsp;&nbsp;&nbsp;&nbsp;&#013;' . t('You have already rated this product.');
  echo '<ul class="star-rating1">
          <li class="current-rating" id="current-rating" style="width:' . ($rating * 25) . 'px"></li>
          <li><a title="' . $title . '"  class="one-star">1</a></li>
          <li><a title="' . $title . '"  class="two-stars">2</a></li>
          <li><a title="' . $title . '"  class="three-stars">3</a></li>
          <li><a title="' . $title . '"  class="four-stars">4</a></li>
          <li><a title="' . $title . '"  class="five-stars">5</a></li>
        </ul>';
}
