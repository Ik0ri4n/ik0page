<script lang="ts">
	import type { Post } from '$lib/types/post';
	import type { ComponentType } from 'svelte';

	export let Content: ComponentType;
	export let meta: Post;
</script>

<article class="mx-auto max-w-3xl">
	<h1 class="text-2xl font-medium text-gray-900 dark:text-gray-50 sm:text-3xl">{meta.title}</h1>
	<h2 class="mt-1 text-sm text-gray-500 dark:text-gray-400 sm:text-base">{meta.date}</h2>

	<div
		class="prose prose-gray mt-8 max-w-none prose-a:text-green-700 prose-a:no-underline hover:prose-a:text-green-800 prose-code:font-mono
	prose-code:font-light prose-code:not-italic prose-code:text-purple-700 prose-code:before:contents prose-code:after:contents
	prose-img:mx-auto prose-img:rounded-lg dark:prose-invert dark:prose-a:text-green-500 dark:hover:prose-a:text-green-600 dark:prose-code:text-orange-500 dark:prose-img:brightness-90"
	>
		<svelte:component this={Content} />
	</div>

	<div class="mt-6 pt-2 mb-4 flex flex-nowrap justify-between space-x-2 border-t border-gray-300 dark:border-gray-700">
		<span>
			{#if meta.previous}
				<a class="group flex" data-sveltekit-reload href={meta.previous?.slug}>
					<div
						class="pr-4 text-lg font-bold text-gray-700 transition-transform group-hover:-translate-x-1 group-hover:text-green-800 dark:text-gray-200 dark:group-hover:text-green-600"
					>
						←
					</div>
					<div>
						<h2
							class="text-lg font-medium text-gray-700 group-hover:text-green-800 dark:text-gray-200 dark:group-hover:text-green-600"
						>
							{meta.previous?.title}
						</h2>
						<p class="text-sm text-gray-500 dark:text-gray-400">{meta.previous?.date}</p>
					</div>
				</a>
			{/if}
		</span>
		<span>
			{#if meta.next}
				<a class="group flex text-right" data-sveltekit-reload href={meta.next?.slug}>
					<div>
						<h2
							class="text-lg font-medium text-gray-700 group-hover:text-green-800 dark:text-gray-200 dark:group-hover:text-green-600"
						>
							{meta.next?.title}
						</h2>
						<p class="text-sm text-gray-500 dark:text-gray-400">{meta.next?.date}</p>
					</div>
					<div
						class="pl-4 text-lg font-bold text-gray-700 transition-transform group-hover:translate-x-1 group-hover:text-green-800 dark:text-gray-200 dark:group-hover:text-green-600"
					>
						→
					</div>
				</a>
			{/if}
		</span>
	</div>
</article>
